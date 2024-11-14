//
// Created by yawaflua on 30/10/2024.
//
#pragma once
#ifndef OREFALERTRESPONSE
#define OREFALERTRESPONSE

#include "models/Interfaces/IAlertResponse.cpp"
#include "utils/OREFToTzevaAdom.cpp"
#include <nlohmann/json.hpp>

namespace tzeva_adom{

    class OrefAlert : public IAlert {
    public:
        OrefAlert() = default;
        //virtual ~OrefAlert() = default;

        void set_values(int _threat, std::vector<std::string> _cities) {
            this->cities = _cities;
            this->threat = _threat;
        }

        std::vector<std::string> get_cities() override { return cities; }
        int get_threat() override { return threat; }

    private:
        int threat;
        std::vector<std::string> cities;
    };

    class OrefAlertResponse : public IAlertResponse {
    public:
        OrefAlertResponse(const std::string& alertDate, const std::string& title,
                      const std::string& data, int64_t threat)
        : alert_date(alertDate), title(title), data(data), threat(threat) {}
        virtual ~OrefAlertResponse() = default;

    private:
        std::string alert_date;
        std::string title;
        std::string data;
        int64_t threat;

    public:

        inline void set_data(std::string alert, std::string title_provided, std::string data_provided, int threat_provided) {
            alert_date = alert;
            title = title_provided;
            data = data_provided;
            threat = threat_provided;
        }

        const std::string & get_alert_date() const { return alert_date; }
        std::string & get_mutable_alert_date() { return alert_date; }
        void set_alert_date(const std::string & value) { this->alert_date = value; }

        const std::string & get_title() const { return title; }
        std::string & get_mutable_title() { return title; }
        void set_title(const std::string & value) { this->title = value; }

        const std::string & get_data() const { return data; }
        std::string & get_mutable_data() { return data; }
        void set_data(const std::string & value) { this->data = value; }

        const int64_t &get_threat() const { return threat; }
        int get_threat() override { return oref_threat_to_tzeva_adom(threat); }
        int64_t & get_mutable_threat() { return threat; }
        void set_threat(const int64_t & value) { this->threat = value; }

        int get_id() override {
            std::tm t = {};
            std::istringstream ss(alert_date);

            if (ss >> std::get_time(&t, "%Y-%m-%d %H:%M:%S"))
            {
                return std::mktime(&t) % 1000000;
            }
        };

        std::string get_cities() override {return data; };

        std::vector<std::string> get_cities_arr() override { return std::vector {data}; }

        std::vector<IAlert> get_alerts() override {
            auto orefAlertReturn = new OrefAlert();
            orefAlertReturn->set_values(threat, get_cities_arr());
            IAlert iAlertToReturn = static_cast<IAlert>(std::ref(*orefAlertReturn));
            return std::vector {iAlertToReturn};
        };
    };


}


namespace tzeva_adom {
    // void from_json(const nlohmann::json & j, OrefAlertResponse & x);
    // void to_json(nlohmann::json & j, const OrefAlertResponse & x);

    inline void to_json(nlohmann::json& j, const OrefAlertResponse& alert) {
        j = nlohmann::json{
            {"alert_date", alert.get_alert_date()},
            {"title", alert.get_title()},
            {"data", alert.get_data()},
            {"threat", alert.get_threat()}
        };
    }

    inline void from_json(const nlohmann::json& j, OrefAlertResponse& alert) {
        if (j.contains("alert_date")) {
            alert.set_alert_date(j.at("alert_date").get<std::string>());
        }
        if (j.contains("title")) {
            alert.set_title(j.at("title").get<std::string>());
        }
        if (j.contains("data")) {
            alert.set_data(j.at("data").get<std::string>());
        }
        if (j.contains("threat")) {
            alert.set_threat(j.at("threat").get<int64_t>());
        }
    }
}
#endif
