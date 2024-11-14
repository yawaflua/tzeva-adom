cmake_minimum_required(VERSION 3.10)
project(tzeva_adom)

# Указываем стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Находим пакеты CURL и nlohmann_json
find_package(CURL REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)

# Определяем исполняемый файл
add_executable(tzeva_adom main.cpp
        models/AlertResponse.cpp
        utils/image_downloader.cpp
        utils/audio_play.cpp
        locale/localization_get.cpp
        utils/localization_manager.h
        models/OrefAlertResponse.cpp
        CONFIG.cpp
        models/Interfaces/IAlertResponse.cpp
        models/Interfaces/IAlert.cpp
        locale/OREF_localization.cpp
        models/AlertMessage_OREF.cpp
        utils/OREFToTzevaAdom.cpp
        models/OrefAlertResponse.cpp
)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(NOTIFY REQUIRED libnotify)
pkg_check_modules(SDL REQUIRED libnotify)
find_package(fmt REQUIRED)

include_directories(lang)
include_directories(${GLIB_INCLUDE_DIRS})
include_directories(${NOTIFY_INCLUDE_DIRS})
include_directories(${CURL_INCLUDE_DIRS})

find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_MIXER_INCLUDE_DIRS})
find_package(Boost REQUIRED COMPONENTS filesystem)
include_directories(${Boost_INCLUDE_DIRS})


# Линкуем библиотеки
target_include_directories(tzeva_adom PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} "lang/")
target_link_libraries(tzeva_adom PRIVATE SDL2 SDL2_mixer ${Boost_LIBRARIES} CURL::libcurl nlohmann_json::nlohmann_json ${GLIB_LIBRARIES} fmt::fmt ${NOTIFY_LIBRARIES} ${CURL_LIBRARIES})

add_custom_command(TARGET tzeva_adom POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/lang $<TARGET_FILE_DIR:tzeva_adom>/lang
)