#pragma once
#include <iostream>
#include <string>
#include <curl/curl.h>
#include <boost/json.hpp>
#include <spdlog/spdlog.h>

namespace json = boost::json;

// Callback function
size_t WriteCallback1(void* contents, size_t size, size_t nmemb, std::string* output);

// Authentication function
std::string authenticate(const std::string& client_id, const std::string& client_secret);
