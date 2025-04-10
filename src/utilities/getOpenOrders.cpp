#include "getOpenOrders.hpp"

size_t WriteCallback4(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total_size = size * nmemb;
    output->append((char*)contents, total_size);
    return total_size;
}

std::vector<OrderInfo> getOpenOrders(const std::string& access_token) {
    CURL* curl;
    CURLcode res;
    std::string response_buffer;
    std::vector<OrderInfo> open_orders;

    spdlog::info("Fetching open orders...");

    curl = curl_easy_init();
    if (curl) {
        std::string url = "https://test.deribit.com/api/v2/private/get_open_orders";
        struct curl_slist* headers = nullptr;
        std::string auth_header = "Authorization: Bearer " + access_token;
        headers = curl_slist_append(headers, auth_header.c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback4);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_buffer);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            spdlog::error("Error fetching open orders: {}", curl_easy_strerror(res));
            return open_orders;
        }
        
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    try {
        boost::json::value response_json = boost::json::parse(response_buffer);
        if (response_json.as_object().contains("result")) {
            auto orders = response_json.at("result").as_array();
            spdlog::info("Fetched {} open orders", orders.size());

            int index = 1;
            std::cout << "Open Orders:\n";
            for (const auto& order : orders) {
                OrderInfo order_info;
                order_info.order_id = order.at("order_id").as_string().c_str();
                order_info.instrument = order.at("instrument_name").as_string().c_str();
                order_info.price = order.at("price").as_double();
                order_info.amount = order.at("amount").as_double();
                order_info.order_type = order.at("order_type").as_string().c_str();

                open_orders.push_back(order_info);

                // Display order in simple format
                std::cout << index++ << ". Order ID: " << order_info.order_id
                          << " | Instrument: " << order_info.instrument
                          << " | Type: " << order_info.order_type
                          << " | Price: " << order_info.price
                          << " | Amount: " << order_info.amount
                          << std::endl;
            }
        }
    } catch (const std::exception& e) {
        spdlog::error("JSON Parsing Error in getOpenOrders: {}", e.what());
    }

    return open_orders;
}
