//
// Created by yawaflua on 30/10/2024.
//
#pragma once
#include <vector>
#include <string>

namespace tzeva_adom {
    class IAlert {
    public:
        virtual ~IAlert() = default;

        virtual std::vector<std::string> get_cities() {};
        virtual int get_threat() {};
    };
}