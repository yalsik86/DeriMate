#include "placeOrder.hpp"

size_t WriteCallback2(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total_size = size * nmemb;
    output->append((char*)contents, total_size);
    return total_size;
}

// Place Order Function
bool placeOrder(std::string& access_token, std::string &instrument) {
    CURL* curl;
    CURLcode res;
    std::string response_buffer;

    spdlog::info("Placing order for instrument: {}", instrument);

    // Select order direction
    std::string order_side;
    int side_choice;
    std::cout << "Select Order Side:\n";
    std::cout << "0. Abort Order Placement\n";
    std::cout << "1. Buy\n";
    std::cout << "2. Sell\n";
    std::cout << "Enter choice: ";
    std::cin >> side_choice;

    if (side_choice == 0) {
        spdlog::warn("Order placement aborted by user.");
        return false;
    }
    if (side_choice == 1) {
        order_side = "buy";
    } else if (side_choice == 2) {
        order_side = "sell";
    } else {
        spdlog::error("Invalid order side selection.");
        return false;
    }

    // Select order type
    std::string order_type;
    int type_choice;
    std::cout << "Select Order Type:\n";
    std::cout << "0. Abort Order Placement\n";
    std::cout << "1. Market\n";
    std::cout << "2. Limit\n";
    std::cout << "Enter choice: ";
    std::cin >> type_choice;

    if (type_choice == 0) {
        spdlog::warn("Order placement aborted by user.");
        return false;
    }
    if (type_choice == 1) {
        order_type = "market";
    } else if (type_choice == 2) {
        order_type = "limit";
    } else {
        spdlog::error("Invalid order type selection.");
        return false;
    }

    // Enter amount
    double amount;
    std::cout << "Enter trade amount (or 0 to abort): ";
    std::cin >> amount;
    if (amount == 0) {
        spdlog::warn("Order placement aborted by user.");
        return false;
    }

    // Enter limit price if applicable
    double price = 0.0;
    if (order_type == "limit") {
        std::cout << "Enter limit price (or 0 to abort): ";
        std::cin >> price;
        if (price == 0) {
            spdlog::warn("Order placement aborted by user.");
            return false;
        }
    }

    // Construct API URL
    curl = curl_easy_init();
    if (!curl) {
        spdlog::error("CURL initialization failed.");
        return false;
    }

    std::string url = "https://test.deribit.com/api/v2/private/" + order_side +
                      "?instrument_name=" + instrument +
                      "&amount=" + std::to_string(amount) +
                      "&type=" + order_type;

    if (order_type == "limit") {
        url += "&price=" + std::to_string(price);
    }

    struct curl_slist* headers = nullptr;
    std::string auth_header = "Authorization: Bearer " + access_token;
    headers = curl_slist_append(headers, auth_header.c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback2);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_buffer);

    spdlog::info("Sending order request: Side={}, Type={}, Amount={}, Instrument={}", 
                  order_side, order_type, amount, instrument);

    res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        spdlog::error("Order Placement Error: {}", curl_easy_strerror(res));
        return false;
    }

    // Parse JSON response
    try {
        boost::json::value response_json = boost::json::parse(response_buffer);

        // Check if there's an error
        if (response_json.as_object().contains("error")) {
            auto error_data = response_json.at("error").as_object();
            std::string error_message = error_data.at("message").as_string().c_str();
            
            std::string reason;
            if (error_data.contains("data")) {
                auto data_obj = error_data.at("data").as_object();
                if (data_obj.contains("reason")) {
                    reason = data_obj.at("reason").as_string().c_str();
                }
            }

            spdlog::error("Order Placement Failed: {}", error_message);
            if (!reason.empty()) {
                spdlog::warn("Failure Reason: {}", reason);
            }
            return false;
        }

        auto order_data = response_json.at("result").as_object().at("order").as_object();

        // Extract order details
        std::string order_id = order_data.at("order_id").as_string().c_str();
        std::string order_status = order_data.at("order_state").as_string().c_str();

        spdlog::info("Order Placed Successfully - Order ID: {}, Status: {}", order_id, order_status);
        return true;
    }
    catch (const std::exception& e) {
        spdlog::error("JSON Parsing Error: {}", e.what());
        return false;
    }
}
