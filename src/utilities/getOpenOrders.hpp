#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <curl/curl.h>
#include <spdlog/spdlog.h>
#include <boost/json.hpp>

struct OrderInfo {
    std::string order_id;
    std::string order_type;
    std::string instrument;
    double price;
    double amount;
};

std::vector<OrderInfo> getOpenOrders(const std::string& access_token);
