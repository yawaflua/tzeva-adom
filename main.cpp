#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <curl/curl.h>
#include <atomic>
#include <nlohmann/json.hpp>
#include "spdlog/spdlog.h"
#include <string_view>
#include <libnotify/notify.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>

#include "utils/audio_play.cpp"
#include "models/AlertResponse.cpp"
#include "utils/image_downloader.cpp"
#include "utils/localization_manager.h"

class AlertResponse;

tzeva_adom::LocalizationManager localization_manager;
using json = nlohmann::json;
int16_t lastId = 0;
json cities_n_areas_list;
bool is_cities_loaded = false;
bool is_test = false;
std::string this_path = boost::filesystem::current_path().c_str();
std::string lang = "en";
std::vector<tzeva_adom::Alert> test_alert_variable;

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
        auto* first_alert = new tzeva_adom::AlertResponseElement(response.at(0));

        spdlog::debug("Answer: {}", first_alert->get_id());

        auto id = first_alert->get_id();

        if (lastId == 0 || id == lastId) {
            lastId = id;
        } else {
            is_alert = !is_alert;
            lastId = id;
            spdlog::debug("This is alert!");
        }

        if (is_alert || is_test) {
            is_test = !is_test;
            // auto description = first_alert["description"].is_string() ?"": first_alert["description"].get<std::string>() ;
            std::string description = "";
            const std::vector<tzeva_adom::Alert> alerts_array = is_test ? test_alert_variable : first_alert->get_alerts();
            std::vector<std::string> cities;
            std::vector<int> threats;

            for (auto alerts: alerts_array) {
                auto time = alerts.get_time();
                for (auto city: alerts.get_cities()) {
                    cities.insert(cities.begin(), city);
            }

            threats.insert(threats.begin(), alerts.get_threat());
            auto isDrill = alerts.get_is_drill();



            spdlog::debug("Time: {}", time);
            spdlog::debug("Cities: {}", fmt::join(cities, ", "));

            spdlog::debug("Threat Level: {}", alerts.get_threat());
            spdlog::debug("Is Drill: {}", isDrill);
            }

            int threat = *std::max_element( threats.begin(), threats.end() );
            std::string icon_url = boost::filesystem::current_path().c_str()+fmt::format("/threat{}.{}", std::to_string(threat), threat == 0 ? "png" : "svg");

            spdlog::debug("Threat: {}", threat);
            spdlog::debug("Threat name: {}", localization_manager.getString(fmt::format("threat_{}", threat)));
            spdlog::debug("Language: {}", lang);
            spdlog::debug("Icon path: {}", icon_url);

            std::string localised_cities_names = "";
            for (auto city: cities) {
                localised_cities_names += fmt::format("{} ", cities_n_areas_list["cities"][city][lang]);
            }

            spdlog::debug("Init notification");
            notify_init(localization_manager.getString(fmt::format("threat_{}", std::to_string(threat))).c_str());
            NotifyNotification* n = notify_notification_new (localization_manager.getString(fmt::format("threat_{}", std::to_string(threat))).c_str(),
                        fmt::format(
                            "{}: {}\n"\
                            "{}: {}\n"\
                            "{}: {}",
                            localization_manager.getString("cities"),
                            localised_cities_names,
                            localization_manager.getString("threat"),
                            localization_manager.getString(fmt::format("threat_{}", std::to_string(threat))),
                            localization_manager.getString("drill"),
                            localization_manager.getString(is_drill ? "true" : "false")
                            ).c_str(),
                            icon_url.c_str()
                            );
            tzeva_adom::playAudioAsync((this_path + std::string("/bell.mp3")).c_str());
            notify_notification_set_timeout(n, 10000); // 10 seconds

            if (!notify_notification_show(n, 0))
            {
                std::cerr << "show has failed" << std::endl;
                return;
            }
            return;
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
            curl_global_cleanup();
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

    std::string last_flag = "";
    bool is_accept_values = false;

    for (int i = 0; i < argc; i++) {

        if (argv[i] == "-d"sv || argv[i] == "--debug")
            spdlog::set_level(spdlog::level::debug); // Set global log level to debug if provided --debug
        else if (argv[i] == "-t"sv || argv[i] == "--test") {
            is_test = true; // Set tested variable is true
            std::vector alert_to_array {tzeva_adom::Alert()};
            const auto p1 = std::chrono::system_clock::now();
            ulong t = std::chrono::duration_cast<std::chrono::seconds>(
                   p1.time_since_epoch()).count();
            std::vector<std::string> city_vectors{"אשדוד -יא,יב,טו,יז,מרינה,סיט", "אשדוד - ח,ט,י,יג,יד,טז"};
            alert_to_array[0].set_cities(city_vectors);
            alert_to_array[0].set_time(t);
            alert_to_array[0].set_threat(0);
            test_alert_variable.push_back(alert_to_array[0]);
        }
        else if (argv[i] == "-l"sv || argv[i] == "--lang"sv) {
            is_accept_values = true;
            spdlog::debug("Take lang arg");
        }
        else if ((last_flag == "-l"sv || last_flag == "--lang"sv) && is_accept_values == true) {
            is_accept_values = false;
            lang = argv[i];
            spdlog::debug("Language setted to {}", lang);

        }
        else if (argv[i] == "-h"sv || argv[i] == "--help"sv) {
            spdlog::info(
                "Tzeva-Adom PC 1.0 by yawaflua\n\n"\
                "Flags: \n"\
                "   -h --help:   Show this message\n"\
                "   -d --debug:  Show debug messages\n"\
                "   -t --test:   Create test alert end exit\n"\
                "   -l --lang:   Choose language: ru, en, he,"
                "");
            return 0;
        }
        last_flag = argv[i];
    }

    spdlog::debug("Path: {}", boost::filesystem::current_path().c_str());

    for (int i = 0; i <= 5; i++) {
        if (!boost::filesystem::exists(this_path+fmt::format("/threat{}.{}", std::to_string(i), i == 0 ? "png" : "svg"))) {
            tzeva_adom::download_file(
                fmt::format(
                    "https://www.tzevaadom.co.il/static/images/threat{}.{}",
                    std::to_string(i), i == 0 ? "png" : "svg"),
                fmt::format("threat{}.{}", std::to_string(i), i == 0 ? "png" : "svg")
                );

        }
    }
    localization_manager = tzeva_adom::LocalizationManager();
    localization_manager.setCurrentLanguage(lang);
    //Download tzeva-adom bell sound
    if (!boost::filesystem::exists(this_path+fmt::format("/bell.mp3"))) {
        tzeva_adom::download_file(
            "https://www.tzevaadom.co.il/static/sounds/bell.mp3",
            fmt::format("bell.mp3")
            );
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
