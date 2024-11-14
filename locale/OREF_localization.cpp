//
// Created by yawaflua on 01/11/2024.
//
#include <unistd.h>
#include <spdlog/fmt/fmt.h>
#ifndef OREF_LOCALIZATION_CPP
#define OREF_LOCALIZATION_CPP

#include "models/AlertMessage_OREF.cpp"
#include <string>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp> // Подключение библиотеки JSON
#include <curl/curl.h>
#include "./CONFIG.cpp"

namespace  tzeva_adom {
    //class AlertMessage;

    class LocalizationManager {
    private:
        std::vector<AlertMessage> jsonAlertsTranslate;
        size_t static WriteCallbackAlerts(void* contents, size_t size, size_t nmemb, std::string* output) {
            output->append(static_cast<char*>(contents), size * nmemb);
            return size * nmemb;
        }

        std::string loadLocalizations(std::string lang) {
            CURL* curl = curl_easy_init();
            std::string readBuffer;
            curl_easy_setopt(curl, CURLOPT_URL, ALERT_TRANSLATIONS_OREF);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallbackAlerts);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
            CURLcode res = curl_easy_perform(curl);
            auto jsonResponseFromLocalization = nlohmann::json::parse(readBuffer);
            jsonAlertsTranslate = jsonResponseFromLocalization.get<std::vector<AlertMessage>>();

            curl_easy_cleanup(curl);
        }

    public:
        LocalizationManager() = default;
        virtual ~LocalizationManager() = default;

        // Загрузить языковой файл
        bool loadLanguage(std::string lang) {
            std::string path = std::filesystem::current_path().c_str() + fmt::format("/lang/{}.json", lang);
            std::ifstream file(path);
            if (!file.is_open()) {
                std::cerr << "Failed to open localization file: " << lang << std::endl;
                std::cerr << "Failed to open localization file: " << path << std::endl;
                return false;
            }

            nlohmann::json json;
            file >> json;
            translations[lang] = json;
            return true;
        }

        // Установить текущий язык
        void setCurrentLanguage(const std::string& langCode) {
            loadLanguage(langCode);
            if (translations.find(langCode) != translations.end()) {
                currentLanguage = langCode;

            } else {
                std::cerr << "Language code not loaded: " << langCode << std::endl;
            }
        }

        // Получить переведенную строку по ключу
        std::string getString(const std::string& key) const {
            if (!translations.find(currentLanguage)->second.empty()) {
                auto& langData = translations.at(currentLanguage);
                if (langData.contains(key)) {
                    return langData[key];
                }
            }
            return "Translation not found";
        }

    private:
        std::unordered_map<std::string, nlohmann::json> translations;
        std::string currentLanguage;
    };
}
#endif // LOCALIZATION_MANAGER_H
