#ifndef LOCALIZATION_MANAGER_H
#define LOCALIZATION_MANAGER_H

#include <string>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp> // Подключение библиотеки JSON

namespace  tzeva_adom {
    class LocalizationManager {
    public:
        LocalizationManager() : currentLanguage("en") {}

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
