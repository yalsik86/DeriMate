#pragma once
#include <string>
#include <curl/curl.h>
#include <iostream>
#include <boost/json.hpp>
#include <spdlog/spdlog.h>

namespace json = boost::json;

// Dedicated callback function for processing response data
size_t WriteCallback3(void* contents, size_t size, size_t nmemb, std::string* output);

// Function to cancel an order
bool cancelOrder(const std::string& access_token, std::vector<OrderInfo>& open_orders);
