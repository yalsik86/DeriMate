# **DERIMATE**

## **Overview**
This is a high-performance **Order execution and management system** CLI with **real-time market data streaming** capability on Deribit Testnet.

### **FEATURES:**

- **Order Management** – Place, cancel, modify orders & view positions  
- **Market Data Streaming** – Real-time WebSocket streaming for order books   
- **Logging & Debugging** – Uses `spdlog` for structured logging  
- **High Performance** – Designed with `Boost.Asio` and `CURL` for efficient API communication
- **Authentication** – To generate access token for API access   

---

## **SETUP**

### **Prerequisites**
This project was developed using the **MSYS2** environment, but you can use any setup that suits you. 

Ensure you have the following dependencies installed:
- `Boost` for JSON/WebSocket handling  
- `cURL` for HTTP API requests  
- `spdlog` for logging and debugging 

Also add your Deribit Testnet **API Scret_Key** and **Client_ID** to `credentials.hpp` file

Specify necessary linkers before compilation `-lcurl -lssl -lcrypto -lws2_32 -lspdlog -lfmt`

---

## **FOLDER STRUCTURE**
```
📂 src
├── 📂 include/                # Market Data Streaming Module  
│   ├── 📄 credentials.hpp       # API Secret Key and Client_ID
│   └── 📄 methods.cpp           # Contains method implementations  
│
├── 📂 marketDataStream/       # Market Data Streaming Module  
│   └── 📄 streamOrderbook.cpp  # Streams real-time order book data  
│  
├── 📂 orderManagement/        # Order Execution & Management  
│   ├── 📄 placeOrder.cpp       # Places a new order  
│   ├── 📄 cancelOrder.cpp      # Cancels an order  
│   ├── 📄 modifyOrder.cpp      # Modifies an existing order  
│   ├── 📄 viewCurrentPositions.cpp  # Retrieves open positions  
│   └── 📄 getOrderbook.cpp     # Fetches order book data  
│  
├── 📂 utilities/              # Utility Functions  
│   ├── 📄 authentication.cpp   # Handles OAuth authentication  
│   ├── 📄 getCurrency.cpp      # Fetches available currencies  
│   ├── 📄 getInstrument.cpp    # Gets instrument details  
│   └── 📄 getOpenOrders.cpp    # Retrieves open orders  
│  
└── 📄 main.cpp                 # Main execution file
```
