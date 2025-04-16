#include "streamOrderbook.hpp"

std::atomic<bool> keep_running(true);

void subscribeOrderBook(websocket::stream<beast::ssl_stream<beast::tcp_stream>>& ws, std::string &instrument) {
    json::value subscription_request = {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "public/subscribe"},
        {"params", {{"channels", json::array{"book." + instrument + ".agg2"}}}}
    };

    ws.write(asio::buffer(json::serialize(subscription_request)));
    spdlog::info("Subscribed to Order Book for {}", instrument);
}

void receiveOrderBook(websocket::stream<beast::ssl_stream<beast::tcp_stream>>& ws) {
    beast::flat_buffer buffer;
    int max_levels = 3;

    try {
        while (keep_running) {
            ws.read(buffer);  // Blocking read

            std::string data = beast::buffers_to_string(buffer.data());
            buffer.clear();

            try {
                json::value response = json::parse(data);

                if (response.as_object().if_contains("params")) {
                    auto params = response.as_object()["params"].as_object();
                    if (params.if_contains("data")) {
                        auto data_obj = params["data"].as_object();

                        if (data_obj.if_contains("bids")) {
                            int count = 0;
                            for (auto& bid : data_obj["bids"].as_array()) {
                                if (count++ >= max_levels) break;
                                auto bid_array = bid.as_array();
                                spdlog::info("Bid: {} - [{} @ {}]",
                                            std::string(bid_array[0].as_string()),  // Convert boost::json::string to std::string
                                            bid_array[1].as_double(),
                                            bid_array[2].as_double());
                            }
                        }

                        if (data_obj.if_contains("asks")) {
                            int count = 0;
                            for (auto& ask : data_obj["asks"].as_array()) {
                                if (count++ >= max_levels) break;
                                auto ask_array = ask.as_array();
                                spdlog::info("Ask: {} - [{} @ {}]",
                                            std::string(ask_array[0].as_string()),  // Convert boost::json::string to std::string
                                            ask_array[1].as_double(),
                                            ask_array[2].as_double());
                            }
                        }
                    }
                }
            } catch (const std::exception& e) {
                spdlog::error("JSON Parsing Error: {}", e.what());
            }
        }
    } catch (const std::exception& e) {
        spdlog::error("WebSocket Read Error: {}", e.what());
    }
    std::cout<<std::endl;
    spdlog::info("Stopping receiveOrderBook thread...");
}

void connectToDeribit(asio::io_context& ioc, ssl::context& ctx, websocket::stream<beast::ssl_stream<beast::tcp_stream>>& ws) {
    try {
        tcp::resolver resolver(ioc);
        auto const results = resolver.resolve("test.deribit.com", "443");

        beast::get_lowest_layer(ws).connect(results);

        ws.next_layer().handshake(ssl::stream_base::client);
        ws.handshake("test.deribit.com", "/ws/api/v2");
        
        std::cout<<std::endl;
        spdlog::info("Connected to Deribit WebSocket!");
    } catch (const std::exception& e) {
        spdlog::error("Connection failed: {}", e.what());
    }
}

void disconnectFromDeribit(websocket::stream<beast::ssl_stream<beast::tcp_stream>>& ws) {
    try {
        ws.close(websocket::close_code::normal);
        spdlog::info("Disconnected from Deribit WebSocket.");
    } catch (const std::exception& e) {
        std::string error_msg = e.what();
        if (error_msg.find("The I/O operation has been aborted") != std::string::npos) {
            spdlog::warn("Forcefully closed WebSocket");
        } else {
            spdlog::error("Disconnection error: {}", e.what());
        }
    }
}

bool streamOrderbook(std::string &instrument) {
    keep_running = true;

    asio::io_context ioc;
    ssl::context ctx(ssl::context::tls_client);
    websocket::stream<beast::ssl_stream<beast::tcp_stream>> ws(ioc, ctx);

    connectToDeribit(ioc, ctx, ws);
    subscribeOrderBook(ws, instrument);

    std::thread receiveThread([&]() {
        receiveOrderBook(ws);
        disconnectFromDeribit(ws);
    });

    std::thread exitThread([&]() {
        std::cout << "\n!!! Press 'q' + ENTER to exit... !!!\n\n";
        std::string user_input;

        while (true) {
            std::getline(std::cin, user_input);
            if (user_input == "q") {
                keep_running = false;
                break;
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(2));

        if (ws.is_open()) {
            spdlog::warn("Forcefully closing WebSocket...");
            beast::get_lowest_layer(ws).socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both);
        }
    });

    receiveThread.join();
    exitThread.join();

    return 0;
}
