#include "modifyOrder.hpp"
#include "../utilities/getOpenOrders.hpp"

size_t WriteCallback8(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total_size = size * nmemb;
    output->append((char*)contents, total_size);
    return total_size;
}

bool modifyOrder(const std::string& access_token, std::vector<OrderInfo>& open_orders) {
    if (open_orders.empty()) {
        spdlog::warn("No open orders available to modify.");
        return false;
    }

    size_t choice;
    std::cout << "Enter 0 to abort modification: " << std::endl;
    std::cout << "Enter order number: ";
    std::cin >> choice;

    if (choice == 0) {
        spdlog::warn("Order modification aborted by user.");
        return false;
    }

    if (choice < 1 || choice > open_orders.size()) {
        spdlog::error("Invalid order selection: {}", choice);
        return false;
    }

    auto selected_order = open_orders[choice - 1];

    // Get user input for modifications
    double new_price, new_amount;
    std::cout << "Enter new price: ";
    std::cin >> new_price;
    std::cout << "Enter new amount: ";
    std::cin >> new_amount;

    if (new_price == 0) new_price = selected_order.price;
    if (new_amount == 0) new_amount = selected_order.amount;

    spdlog::info("Modifying order {} with new price {} and new amount {}",
                 selected_order.order_id, new_price, new_amount);

    // Construct API request
    CURL* curl = curl_easy_init();
    if (!curl) {
        spdlog::error("CURL initialization failed.");
        return false;
    }

    std::string url = "https://test.deribit.com/api/v2/private/edit?order_id=" +
                      selected_order.order_id + "&price=" + std::to_string(new_price) +
                      "&amount=" + std::to_string(new_amount);

    std::string response_buffer;
    struct curl_slist* headers = nullptr;
    std::string auth_header = "Authorization: Bearer " + access_token;
    headers = curl_slist_append(headers, auth_header.c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback8);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_buffer);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        spdlog::error("Modify Order Error for order {}: {}", selected_order.order_id, curl_easy_strerror(res));
        return false;
    }

    // Parse response
    try {
        json::value response_json = json::parse(response_buffer);

        if (response_json.as_object().contains("error")) {
            std::string error_message = response_json.at("error").as_object().at("message").as_string().c_str();
            spdlog::error("Error modifying order {}: {}", selected_order.order_id, error_message);
            return false;
        }

        spdlog::info("Order {} modified successfully.", selected_order.order_id);
        return true;
    } catch (const std::exception& e) {
        spdlog::error("JSON Parsing Error in modifyOrder: {}", e.what());
        return false;
    }
}
