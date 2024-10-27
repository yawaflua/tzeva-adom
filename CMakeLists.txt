cmake_minimum_required(VERSION 3.10)
project(tzeva_adom)

# Указываем стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Находим пакеты CURL и nlohmann_json
find_package(CURL REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)

# Определяем исполняемый файл
add_executable(tzeva_adom main.cpp)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(NOTIFY REQUIRED libnotify)
find_package(fmt REQUIRED)

include_directories(${GLIB_INCLUDE_DIRS})
include_directories(${NOTIFY_INCLUDE_DIRS})
include_directories(${CURL_INCLUDE_DIRS})
# Линкуем библиотеки
target_link_libraries(tzeva_adom PRIVATE CURL::libcurl nlohmann_json::nlohmann_json ${GLIB_LIBRARIES} fmt::fmt ${NOTIFY_LIBRARIES} ${CURL_LIBRARIES})
