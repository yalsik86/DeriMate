#include "getCurrency.hpp"

size_t WriteCallback7(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total_size = size * nmemb;
    output->append((char*)contents, total_size);
    return total_size;
}

std::string getCurrency() {
    CURL* curl;
    CURLcode res;
    std::string response_buffer;
    std::vector<std::string> currencies;

    spdlog::info("Fetching supported currencies...");

    std::string url = "https://test.deribit.com/api/v2/public/get_currencies";

    curl = curl_easy_init();
    if (!curl) {
        spdlog::error("Failed to initialize CURL.");
        return "";
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback7);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_buffer);

    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        spdlog::error("Error fetching currencies: {}", curl_easy_strerror(res));
        return "";
    }

    try {
        boost::json::value response_json = boost::json::parse(response_buffer);
        auto result = response_json.at("result").as_array();

        if (result.empty()) {
            spdlog::warn("No currencies found.");
            return "";
        }

        spdlog::info("Fetched {} currencies.", result.size());

        std::cout << "Select a Currency (0 to abort):\n";
        int index = 1;
        for (auto& item : result) {
            std::string currency = item.at("currency").as_string().c_str();
            currencies.push_back(currency);
            std::cout << index << ". " << currency << "\n";
            index++;
        }

        int choice;
        std::cout << "Enter choice: ";
        std::cin >> choice;

        if (choice == 0) {
            spdlog::info("User aborted currency selection.");
            return "";
        } else if (choice > 0 && choice <= currencies.size()) {
            return currencies[choice - 1];
        } else {
            spdlog::error("Invalid currency selection.");
            return "";
        }
    } catch (const std::exception& e) {
        spdlog::error("JSON Parsing Error: {}", e.what());
        return "";
    }
}
