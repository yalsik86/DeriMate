#include <iostream>
#include <string>
#include <curl/curl.h>
#include <boost/json.hpp>
#include <spdlog/spdlog.h>

namespace json = boost::json;

// Callback function
size_t WriteCallback2(void* contents, size_t size, size_t nmemb, std::string* output);

// Order placement function
bool placeOrder(std::string& access_token, std::string &instrument);
