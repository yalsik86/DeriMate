#pragma once

#include <string>
#include <iostream>
#include <boost/json.hpp>
#include <curl/curl.h>
#include <spdlog/spdlog.h>

bool getOrderbook(std::string &instrument, std::vector<OrderInfo> &open_orders);