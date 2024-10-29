//
// Created by yawaflua on 29/10/2024.
//
#pragma once

#include <iostream>
#include <fstream>
#include <curl/curl.h>

namespace tzeva_adom {
    inline size_t writeImageData(void* ptr, size_t size, size_t nmemb, FILE* stream) {
        size_t written = fwrite(ptr, size, nmemb, stream);
        return written;
    }

    inline void download_file(std::string url, std::string filename) {
        CURL* curl = curl_easy_init();
        if (curl) {
            FILE* fp = fopen(filename.c_str(), "wb");
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeImageData);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
            CURLcode res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            fclose(fp);

            if (res != CURLE_OK) {
                std::cout << "Failed to download image: " << curl_easy_strerror(res) << std::endl;
            } else {
                std::cout << "Image downloaded successfully: " << filename << std::endl;
            }
        }
    }
}