#pragma once

#include <string>
#include <iostream>
#include <curl/curl.h>
#include <boost/json.hpp>
#include <spdlog/spdlog.h>

namespace json = boost::json;

bool modifyOrder(const std::string& access_token);
