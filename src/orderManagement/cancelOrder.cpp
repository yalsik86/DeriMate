#include "../utilities/getOpenOrders.hpp"
#include "cancelOrder.hpp"

size_t WriteCallback3(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total_size = size * nmemb;
    output->append((char*)contents, total_size);
    return total_size;
}

bool cancelOrder(const std::string& access_token, std::vector<OrderInfo>& open_orders) {
    if (open_orders.empty()) {
        spdlog::warn("No open orders available to cancel.");
        return false;
    }

    std::cout << "Enter 0 to abort cancellation: " << std::endl;
    std::cout << "Enter order number: ";
    int choice;
    std::cin >> choice;

    if (choice == 0) {
        spdlog::warn("Order cancellation aborted by user.");
        return false;
    }

    if (choice <= 0 || choice > static_cast<int>(open_orders.size())) {
        spdlog::error("Invalid order selection: {}", choice);
        return false;
    }

    std::string order_id = open_orders[choice - 1].order_id;
    spdlog::info("Canceling order: {}", order_id);

    // Proceed with cancellation
    CURL* curl;
    CURLcode res;
    std::string response_buffer;

    curl = curl_easy_init();
    if (curl) {
        std::string url = "https://test.deribit.com/api/v2/private/cancel?order_id=" + order_id;
        struct curl_slist* headers = nullptr;
        std::string auth_header = "Authorization: Bearer " + access_token;
        headers = curl_slist_append(headers, auth_header.c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback3);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_buffer);

        res = curl_easy_perform(curl);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            spdlog::error("Error canceling order {}: {}", order_id, curl_easy_strerror(res));
            return false;
        }
    }

    spdlog::info("Order {} canceled successfully.", order_id);
    return true;
}
