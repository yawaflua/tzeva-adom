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
#include <fmt/format.h>
#include <regex>
#include <nlohmann/json.hpp>
#include "models/Interfaces/IAlertResponse.cpp"
#include "models/Interfaces/IAlert.cpp"

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

    class Alert : public IAlert{
        public:
        Alert() = default;
        Alert(
            const int64_t time,
            const std::vector<std::string>& cities,
            int64_t threat,
            bool is_drill
        ) :
        time(time), cities(cities), threat(threat), is_drill(is_drill) {};
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
        std::vector<std::string> get_cities() override { return cities; }
        std::vector<std::string> & get_mutable_cities() { return cities; }
        void set_cities(const std::vector<std::string> & value) { this->cities = value; }

        const int64_t & get_threat() const { return threat; }
        int get_threat() override { return threat; }
        int64_t & get_mutable_threat() { return threat; }
        void set_threat(const int64_t & value) { this->threat = value; }

        const bool & get_is_drill() const { return is_drill; }
        bool & get_mutable_is_drill() { return is_drill; }
        void set_is_drill(const bool & value) { this->is_drill = value; }
    };

    class AlertResponseElement : public IAlertResponse {
        public:
        AlertResponseElement() = default;
        AlertResponseElement(
            const int id,
            const nlohmann::json description,
            const nlohmann::json alerts)
            : id(id), description(description) {
            for (const auto& alertJson : alerts) {

                int64_t time = alertJson["time"];
                std::vector<std::string> cities = alertJson["cities"].get<std::vector<std::string>>();
                int64_t threat = alertJson["threat"];
                bool is_drill = alertJson["isDrill"];

                Alert _alert = *std::make_unique<Alert>(time, cities, threat, is_drill).get();
                this->alerts.push_back(_alert);
            }
        }
        virtual ~AlertResponseElement() = default;

        private:
        int64_t id;
        nlohmann::json description;
        std::vector<Alert> alerts;

        public:
        int get_id() override { return id; }
        const int64_t &get_id() const { return id; }
        int64_t & get_mutable_id() { return id; }
        void set_id(const int64_t & value) { this->id = value; }

        int get_threat() override {
            int i = 0;
            for (auto alert: alerts) {
                i += alert.get_threat();
            }
            return i / alerts.size();
        }

        const nlohmann::json & get_description() const { return description; }
        nlohmann::json & get_mutable_description() { return description; }
        void set_description(const nlohmann::json & value) { this->description = value; }

        const std::vector<Alert> & get_alerts() const { return alerts; }
        std::vector<Alert> & get_mutable_alerts() { return alerts; }
        void set_alerts(const std::vector<Alert> & value) { this->alerts = value; }

        std::string get_cities() override {
            return fmt::format("{}", fmt::join(get_cities_arr(), ", "));
        }

        std::vector<std::string> get_cities_arr() override {
            std::vector<std::string> cities_to_return = {} ;
            for (auto alert : alerts) {
                cities_to_return.push_back(fmt::format("{}", fmt::join(alert.get_cities(), "")));
            }
            return cities_to_return;
        }

        std::vector<IAlert> get_alerts() override {
            std::vector<IAlert> alertsToReturn {};
            for (IAlert alert : alerts) {
                alertsToReturn.insert(alertsToReturn.begin(), alert);
            }
            return alertsToReturn;
        };
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
