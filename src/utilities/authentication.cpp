#include "authentication.hpp"

size_t WriteCallback1(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total_size = size * nmemb;
    output->append((char*)contents, total_size);
    return total_size;
}

// 1. Authentication 
std::string authenticate(const std::string& client_id, const std::string& client_secret) {
    CURL* curl;
    CURLcode res;
    std::string response_buffer;

    spdlog::info("Starting authentication...");

    curl = curl_easy_init();
    if (curl) {
        std::string url = "https://test.deribit.com/api/v2/public/auth?"
                          "client_id=" + client_id +
                          "&client_secret=" + client_secret +
                          "&grant_type=client_credentials";

        spdlog::debug("Authentication URL: {}", url);

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback1);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_buffer);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            spdlog::error("Authentication Error: {}", curl_easy_strerror(res));
            curl_easy_cleanup(curl);
            return "";
        }

        curl_easy_cleanup(curl);
    }

    // JSON parsing
    try {
        json::value response_json = json::parse(response_buffer);
        if (response_json.as_object().contains("result")) {
            std::string access_token = response_json.at("result").at("access_token").as_string().c_str();
            spdlog::info("Authentication successful");
            return access_token; 
        } else {
            spdlog::error("Response does not contain access_token");
            return "";
        }
    }
    catch (const std::exception& e) {
        spdlog::error("JSON Parsing Error: {}", e.what());
        return "";
    }
}