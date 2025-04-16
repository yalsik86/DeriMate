#include "../utilities/authentication.cpp"
#include "../utilities/getOpenOrders.cpp"
#include "../utilities/getCurrency.cpp"
#include "../utilities/getInstrument.cpp"
#include "../orderManagement/placeOrder.cpp"
#include "../orderManagement/cancelOrder.cpp"
#include "../orderManagement/modifyOrder.cpp"
#include "../orderManagement/getOrderbook.cpp"
#include "../orderManagement/viewCurrentPositions.cpp"
#include "../marketDataStream/streamOrderbook.cpp"
#include "credentials.hpp"

// PLACE ORDER
void _placeOrder(std::string& access_token) {
    std::string currency = getCurrency();
    std::string instrument = getInstrument(currency);
    if(placeOrder(access_token, instrument)) {
        std::cout << "Order placed successfully!\n";
    } else {
        std::cerr << "Failed to place order!\n";
    }
}

// CANCEL
void _cancelOrder(std::string& access_token) {
    auto open_orders = getOpenOrders(access_token);
    if(cancelOrder(access_token, open_orders)) {
        std::cout << "Order cancelation successful!\n";
    } else {
        std::cerr << "Failed to cancel order!\n";
    }
}

// MODIFY
void _modifyOrder(std::string& access_token) {
    auto open_orders = getOpenOrders(access_token);
    if(modifyOrder(access_token, open_orders)) {
        std::cout << "Order modification successful!\n";
    } else {
        std::cerr << "Failed to modify order.\n";
    }
}

// GET ORDERBOOK
void _getOrderbook() {
    std::string currency = getCurrency();
    std::string instrument = getInstrument(currency);
    if(getOrderbook(instrument)) {
        std::cout << "Orderbook fetch successful!\n";
    } else {
        std::cerr << "Failed to fetch orderbook.\n";
    }
}

// CURRENT POSITIONS
void _viewCurrentPositions(std::string& access_token) {
    if(viewCurrentPositions(access_token)) {
        std::cout << "Current Positions fetched successfully!\n";
    } else {
        std::cerr << "Failed to fetch orderbook.\n";
    }
}

// STREAM ORDERBOOK
void _streamOrderbook() {
    std::string currency = getCurrency();
    std::string instrument = getInstrument(currency);
    streamOrderbook(instrument);
}