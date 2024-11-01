//  To parse this JSON data, first install
//
//      Boost     http://www.boost.org
//      json.hpp  https://github.com/nlohmann/json
//
//  Then include this file, and then do
//
//     AlertResponse data = nlohmann::json::parse(jsonString);

#pragma once

#include <boost/optional.hpp>
#include <stdexcept>
#include <regex>
#include <nlohmann/json.hpp>

namespace tzeva_adom {
    using nlohmann::json;

    #ifndef NLOHMANN_UNTYPED_tzeva_adom_HELPER
    #define NLOHMANN_UNTYPED_tzeva_adom_HELPER
    inline json get_untyped(const json& j, const char * property) {
        if (j.find(property) != j.end()) {
            return j.at(property).get<json>();
        }
        return json();
    }

    inline json get_untyped(const json & j, std::string property) {
        return get_untyped(j, property.data());
    }
    #endif

    class Alert {
        public:
        Alert() = default;
        virtual ~Alert() = default;

        private:
        int64_t time;
        std::vector<std::string> cities;
        int64_t threat;
        bool is_drill;

        public:
        const int64_t & get_time() const { return time; }
        int64_t & get_mutable_time() { return time; }
        void set_time(const int64_t & value) { this->time = value; }

        const std::vector<std::string> & get_cities() const { return cities; }
        std::vector<std::string> & get_mutable_cities() { return cities; }
        void set_cities(const std::vector<std::string> & value) { this->cities = value; }

        const int64_t & get_threat() const { return threat; }
        int64_t & get_mutable_threat() { return threat; }
        void set_threat(const int64_t & value) { this->threat = value; }

        const bool & get_is_drill() const { return is_drill; }
        bool & get_mutable_is_drill() { return is_drill; }
        void set_is_drill(const bool & value) { this->is_drill = value; }
    };

    class AlertResponseElement {
        public:
        AlertResponseElement() = default;
        virtual ~AlertResponseElement() = default;

        private:
        int64_t id;
        nlohmann::json description;
        std::vector<Alert> alerts;

        public:
        const int64_t & get_id() const { return id; }
        int64_t & get_mutable_id() { return id; }
        void set_id(const int64_t & value) { this->id = value; }

        const nlohmann::json & get_description() const { return description; }
        nlohmann::json & get_mutable_description() { return description; }
        void set_description(const nlohmann::json & value) { this->description = value; }

        const std::vector<Alert> & get_alerts() const { return alerts; }
        std::vector<Alert> & get_mutable_alerts() { return alerts; }
        void set_alerts(const std::vector<Alert> & value) { this->alerts = value; }
    };

    using AlertResponse = std::vector<AlertResponseElement>;
}

namespace tzeva_adom {
    void from_json(const json & j, Alert & x);
    void to_json(json & j, const Alert & x);

    void from_json(const json & j, AlertResponseElement & x);
    void to_json(json & j, const AlertResponseElement & x);

    inline void from_json(const json & j, Alert& x) {
        x.set_time(j.at("time").get<int64_t>());
        x.set_cities(j.at("cities").get<std::vector<std::string>>());
        x.set_threat(j.at("threat").get<int64_t>());
        x.set_is_drill(j.at("isDrill").get<bool>());
    }

    inline void to_json(json & j, const Alert & x) {
        j = json::object();
        j["time"] = x.get_time();
        j["cities"] = x.get_cities();
        j["threat"] = x.get_threat();
        j["isDrill"] = x.get_is_drill();
    }

    inline void from_json(const json & j, AlertResponseElement& x) {
        x.set_id(j.at("id").get<int64_t>());
        x.set_description(get_untyped(j, "description"));
        x.set_alerts(j.at("alerts").get<std::vector<Alert>>());
    }

    inline void to_json(json & j, const AlertResponseElement & x) {
        j = json::object();
        j["id"] = x.get_id();
        j["description"] = x.get_description();
        j["alerts"] = x.get_alerts();
    }
}
