//
// Created by yawaflua on 30/10/2024.
//
#pragma once
#include <string>
#include <vector>
#include "IAlert.cpp"

namespace tzeva_adom {
    class IAlertResponse
    {
        public:
            // IAlertResponse() = default;
            virtual ~IAlertResponse() = default;

            virtual int get_id() = 0;
            virtual int get_threat() = 0;
            virtual std::string get_cities() = 0;
            virtual std::vector<std::string> get_cities_arr() = 0;
            virtual std::vector<IAlert> get_alerts() = 0;

    };

}