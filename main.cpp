#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <curl/curl.h>
#include <atomic>
#include <complex>
#include <nlohmann/json.hpp>
#include "spdlog/spdlog.h"
#include <string_view>
#include <libnotify/notify.h>

using json = nlohmann::json;
int16_t lastId = 0;
json cities_n_areas_list;
bool is_cities_loaded = false;
// Функция для записи данных от curl
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    output->append(static_cast<char*>(contents), size * nmemb);
    return size * nmemb;
}

// Функция для обработки JSON-ответа
void process_alert(const std::string& data) {
    try {
        bool is_alert = false;
        bool is_png = false;
        bool is_drill = false;

        std::string type_of_threat;
        // Парсим JSON
        spdlog::debug("Parse answer from tzeva-adom");
        json response = json::parse(data);

        if (!response.is_array() || response.empty()) {
            spdlog::error("JSON response is not an array or is empty.");
            return;
        }

        spdlog::debug("Get first object from answer");
        // Извлекаем первый объект из массива
        const json& first_alert = response[0];

        spdlog::debug("Answer: {}", first_alert);

        auto id = first_alert["id"].get<int>();

        if (lastId == 0 || id == lastId) {
            lastId = id;
        } else {
            is_alert = !is_alert;
            spdlog::debug("This is alert!");
        }

        if (is_alert) {
            auto description = first_alert["description"].is_string() ? first_alert["description"].get<std::string>() : "";

            for (auto alerts: first_alert["alerts"]) {
                auto time = alerts["time"].get<long>();
                auto cities = alerts["cities"].get<std::vector<std::string>>();
                auto threat = alerts["threat"].get<int>();
                auto isDrill = alerts["isDrill"].get<bool>();

                spdlog::debug("Time: {}", time);
                spdlog::debug("Cities: {}", fmt::join(cities, ", "));

                spdlog::debug("Threat Level: {}", threat);
                spdlog::debug("Is Drill: {}", isDrill);


                is_png  = threat == 0;
                std::string icon_url = fmt::format("https://www.tzevaadom.co.il/static/images/threat{}.{}", std::to_string(threat), is_png ? ".png" : ".svg");

                switch (threat) {
                    case 0:
                        type_of_threat = "Red Alert";
                        break;
                    case 1:
                        break;
                    case 2:
                        type_of_threat = "Fear of Terrorists infiltration";
                        break;
                    case 3:
                        type_of_threat = "Earthquakes warning";
                        break;
                    case 4:
                        type_of_threat = "Tsunami warning";
                        break;
                    case 5:
                        type_of_threat = "Hostile aircraft intrusion";
                        break;
                    default:
                        type_of_threat = "Unnamed";
                }

                std::string localised_cities_names = "";
                for (auto city: cities) {
                    localised_cities_names += fmt::format("{} ", cities_n_areas_list["cities"][city]["ru"]);
                }

                notify_init("Tzeva Adom!");
                NotifyNotification* n = notify_notification_new (type_of_threat.c_str(),
                            fmt::format(
                                "Cities: {}\n"\
                                "Threat: {}\n"\
                                "Is it drill: {}",
                                localised_cities_names,
                                type_of_threat,
                                std::to_string(is_drill)
                                ).c_str(),
                                icon_url.c_str()
                                );
                notify_notification_set_timeout(n, 10000); // 10 seconds

                if (!notify_notification_show(n, 0))
                {
                    std::cerr << "show has failed" << std::endl;
                    return;
                }
                return;
            }
        }

    } catch (const std::exception& e) {
        spdlog::error("Alert error: {}", e.what());
    }

}

// Асинхронная функция для выполнения запросов
void fetch_alerts_history(std::atomic<bool>& running) {
    while (running) {
        CURL* curl;
        CURLcode res;

        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();

        // Parse cities and areas names on he, ru and en
        if (!is_cities_loaded) {
            std::string readBuffer;
            curl_easy_setopt(curl, CURLOPT_URL, "https://www.tzevaadom.co.il/static/cities.json");
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

            res = curl_easy_perform(curl);

            if (res != CURLE_OK) {
                spdlog::error("get city curl_easy_perform() failed: {}", curl_easy_strerror(res));
            } else {
                cities_n_areas_list = json::parse(readBuffer);
            }
            curl_easy_cleanup(curl);
        }

        if (curl) {
            std::string readBuffer;
            curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0");
            curl_easy_setopt(curl, CURLOPT_URL, "https://api.tzevaadom.co.il/alerts-history");
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

            res = curl_easy_perform(curl);

            if (res != CURLE_OK) {
                spdlog::error("sheduler curl_easy_perform() failed: {}", curl_easy_strerror(res));
                spdlog::debug(readBuffer);
            } else {
                // Обработка полученного JSON
                process_alert(readBuffer);
            }
            curl_easy_cleanup(curl);
        }

        curl_global_cleanup();

        // Задержка в 2 секунды между запросами
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

int main(int argc, char** argv) {
    using namespace std::literals;

    spdlog::set_pattern("%v");
    spdlog::info("=========================================================");
    spdlog::info("");
    spdlog::info("          Welcome to tzeva-adom for linux!");
    spdlog::info("          Maked by https://github.com/yawaflua");
    spdlog::info("");
    spdlog::info("=========================================================");
    spdlog::set_level(spdlog::level::info); // Set global log level to info by default

    for (int i = 0; i < argc; i++) {
        if (argv[i] == "-d"sv || argv[i] == "--debug")
            spdlog::set_level(spdlog::level::debug); // Set global log level to debug if provided --debug
        else if (argv[i] == "-h"sv || argv[i] == "--help"sv) {
            spdlog::info(
                "Tzeva-Adom PC 1.0 by yawaflua\n\n"\
                "Flags: \n"\
                "   -h --help:   Show this message\n"\
                "   -d --debug:  Show debug messages\n"\
                "   -t --test:   Create test alert end exit\n"\
                "");
            return 0;
        }
    }

    // Флаг для контроля остановки потока
    std::atomic<bool> running{true};

    spdlog::debug("Start async thread for fetch_alerts_history");
    // Запускаем асинхронный поток для fetch_alerts_history
    std::thread alerts_thread(fetch_alerts_history, std::ref(running));

    // Простое меню для управления остановкой
    std::cout << "Press Enter to stop the alerts fetching...\n";
    std::cin.get();  // Ожидаем нажатия Enter

    // Устанавливаем флаг остановки и ждем завершения потока
    spdlog::debug("Going to stop the thread.");
    running = false;
    if (alerts_thread.joinable()) {
        alerts_thread.join();
        spdlog::debug("Join to thread.");
    }

    spdlog::info("Alerts fetching stopped.");
    return 0;
}
