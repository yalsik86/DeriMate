#pragma once

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/json.hpp>
#include <spdlog/spdlog.h>
#include <iostream>
#include <iomanip>
#include <thread>
#include <atomic>

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace ssl = asio::ssl;
using tcp = asio::ip::tcp;
namespace json = boost::json;

// Function declarations
void subscribeOrderBook(websocket::stream<beast::ssl_stream<beast::tcp_stream>>& ws);
void receiveOrderBook(websocket::stream<beast::ssl_stream<beast::tcp_stream>>& ws);
void connectToDeribit(asio::io_context& ioc, ssl::context& ctx, websocket::stream<beast::ssl_stream<beast::tcp_stream>>& ws);
void disconnectFromDeribit(websocket::stream<beast::ssl_stream<beast::tcp_stream>>& ws);
bool streamOrderbook(std::string &instrument);