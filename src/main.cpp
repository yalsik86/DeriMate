#include "include/methods.cpp"
#include "spdlog/spdlog.h"

void showMenu() {
    std::cout << "\n===== Select an Option =====" << std::endl;
    std::cout << "1. Place Order" << std::endl;
    std::cout << "2. Cancel Order" << std::endl;
    std::cout << "3. Modify Order" << std::endl;
    std::cout << "4. Get Orderbook" << std::endl;
    std::cout << "5. View Current Positions" << std::endl;
    std::cout << "6. Stream Orderbook" << std::endl;
    std::cout << "0. Exit" << std::endl;
    std::cout << "Enter your choice: ";
}

int main() {
    std::string token = authenticate(client_id, client_secret);

    if(token.empty()) {
        spdlog::error("Authentication Failed.");
        return 1;
    }

    while(true) {
        int choice;
        showMenu();
        std::cin >> choice;
        std::cout << "======= ============ =======\n" << std::endl;
        spdlog::info("User selected option: {}", choice);

        switch(choice) {
            case 1:
                _placeOrder(token);
                break;
            case 2:
                _cancelOrder(token);
                break;
            case 3:
                _modifyOrder(token);
                break;
            case 4:
                _getOrderbook();
                break;
            case 5:
                _viewCurrentPositions(token);
                break;
            case 6:
                _streamOrderbook();
                break;
            case 0:
                spdlog::info("Exiting program...");
                return 0;
            default:
                spdlog::warn("Invalid choice! Please try again.");
        }
    }
    return 0;
}