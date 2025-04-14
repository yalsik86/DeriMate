#include "getOrderbook.hpp"

size_t WriteCallback0(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total_size = size * nmemb;
    output->append((char*)contents, total_size);
    return total_size;
}

bool getOrderbook(std::string &instrument) {
    std::string url = "https://test.deribit.com/api/v2/public/get_order_book?instrument_name=" + instrument;

    CURL* curl;
    CURLcode res;
    std::string response_buffer;

    spdlog::info("Fetching order book for instrument: {}", instrument);

    curl = curl_easy_init();
    if (!curl) {
        spdlog::error("Failed to initialize CURL");
        return false;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback0);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_buffer);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        spdlog::error("Error fetching order book: {}", curl_easy_strerror(res));
        return false;
    }

    if (response_buffer.empty()) {
        spdlog::warn("Empty response received from API.");
        return false;
    }

    try {
        boost::json::value json = boost::json::parse(response_buffer);
        
        if (!json.as_object().contains("result")) {
            spdlog::error("Invalid response: 'result' field is missing.");
            return false;
        }

        auto result = json.at("result");

        spdlog::info("Order book data received successfully.");
        std::cout << "------------------------------------\n";
        std::cout << "Instrument: " << result.at("instrument_name").as_string() << "\n\n";
        std::cout << "Last Price: " << result.at("last_price").as_double() << "\n";
        std::cout << "Mark Price: " << result.at("mark_price").as_double() << "\n";
        std::cout << "Index Price: " << result.at("index_price").as_double() << "\n\n";

        if (!result.as_object().contains("stats")) {
            spdlog::warn("Missing 'stats' field in API response.");
        } else {
            auto stats = result.at("stats");
            std::cout << "24h Volume: " << stats.at("volume").as_double() << "\n\n";
            std::cout << "24h High: " << stats.at("high").as_double() << "\n";
            std::cout << "24h Low: " << stats.at("low").as_double() << "\n\n";
        }

        std::cout << "Best Bid Price: " << result.at("best_bid_price").as_double() << "\n";
        std::cout << "Best Ask Price: " << result.at("best_ask_price").as_double() << "\n\n";

        std::cout << "Best Bid Size: " << result.at("best_bid_amount").as_double() << "\n";
        std::cout << "Best Ask Size: " << result.at("best_ask_amount").as_double() << "\n";
        std::cout << "------------------------------------\n";

        return true;
    } catch (const std::exception& e) {
        spdlog::error("Error parsing order book: {}", e.what());
        return false;
    }
}
