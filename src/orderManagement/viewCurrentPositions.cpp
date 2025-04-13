#include "viewCurrentPositions.hpp"

size_t WriteCallback9(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total_size = size * nmemb;
    output->append((char*)contents, total_size);
    return total_size;
}

// Function to view current positions
bool viewCurrentPositions(std::string& access_token) {
    CURL* curl;
    CURLcode res;
    std::string response_buffer;

    curl = curl_easy_init();
    if (!curl) {
        spdlog::error("CURL initialization failed.");
        return false;
    }

    std::string url = "https://test.deribit.com/api/v2/private/get_positions";
    struct curl_slist* headers = nullptr;
    std::string auth_header = "Authorization: Bearer " + access_token;
    headers = curl_slist_append(headers, auth_header.c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback9);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_buffer);

    res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        spdlog::error("Error fetching positions: {}", curl_easy_strerror(res));
        return false;
    }

    // JSON parsing
    try {
        json::value response_json = json::parse(response_buffer);

        if (response_json.as_object().contains("error")) {
            std::string error_message = response_json.at("error").as_object().at("message").as_string().c_str();
            spdlog::error("Error fetching positions: {}", error_message);
            return false;
        }

        auto positions = response_json.at("result").as_array();

        if (positions.empty()) {
            spdlog::info("No open positions.");
            return false;
        }

        spdlog::info("Fetching current positions...");

        std::cout << "Current Positions:\n";
        int index = 1;
        for (const auto& position : positions) {
            std::string instrument_name = position.at("instrument_name").as_string().c_str();
            std::string kind = position.at("kind").as_string().c_str();
            double size = position.at("size").as_double();
            double total_pnl = position.at("total_profit_loss").as_double();

            std::cout << index++ << ". Instrument: " << instrument_name
                      << " | Kind: " << kind
                      << " | Size: " << size
                      << " | Total PnL: " << total_pnl
                      << std::endl;
        }
        return true;
    }
    catch (const std::exception& e) {
        spdlog::error("JSON Parsing Error in viewCurrentPositions: {}", e.what());
        return false;
    }
}
