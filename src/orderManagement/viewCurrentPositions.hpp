#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <curl/curl.h>
#include <boost/json.hpp>
#include <spdlog/spdlog.h>

namespace json = boost::json;

bool viewCurrentPositions(const std::string& access_token);
