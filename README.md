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
│   ├── 📄 credentials.hpp      # API Secret Key and Client_ID
│   └── 📄 methods.cpp          # Contains method implementations  
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
---
---

# **Order Management Module**

## **Overview**
The `orderManagement` module is responsible for handling trade orders on Deribit Testnet. It provides functionalities for placing, modifying, and canceling orders, retrieving open orders and orderbook data at an instance.

This module integrates `cURL` for API communication, `Boost.JSON` for parsing responses, and `spdlog` for logging order-related activities.

---

## **1. Place Order**

It is responsible for executing trade orders on Deribit Testnet. It allows the user to:
- Select **order side** (Buy/Sell)
- Choose **order type** (Market/Limit)
- Enter **trade amount**
- Enter **limit price** (if applicable)
- Send an API request to place the order

---

## **2. Cancel Order**

It allows users to cancel an existing open order on Deribit Testnet. It provides the user with:
- A list of **open orders**
- Ability to **select an order** for cancellation
- Send an API request to cancel the order

---

## **3. Modify Order**

It allows users to modify an existing open order on Deribit Testnet. It provides the user with:
- A list of **open orders**
- Ability to **select an order** for modification
- User input for **new price** and **new amount**
- Send an API request to cancel the order

---

## **4. View Current Positions**

It retrieves and displays the user's open positions on Deribit Testnet. It allows the user to:
- Fetch **current open positions** from the API
- Parse **instrument name**, **type**, **size**, and **total PnL**
- Display **position details** using **spdlog**

---

## **5. Get Order Book**

It retrieves real-time order book data for a given instrument from Deribit Testnet. It allows the user to:
- Fetch **bid-ask prices** and **order book statistics**
- Display **last price**, **mark price**, and **index price**
- Parse and display **24h high, low, and volume**

---
---

# **Market Data Stream Module**

## **Overview**
The `marketDataStream` module is responsible for continuously streaming real-time market data from Deribit Testnet. It provides functionalities for subscribing to order book updates, receiving bid-ask prices, and efficiently handling WebSocket connections.

This module integrates `Boost.Beast` and `Boost.Asio` for WebSocket communication, `Boost.JSON` for parsing incoming data, and `spdlog` for logging real-time market events.

---

# **Functional Components**

## **1. Stream Order Book**

This function establishes a WebSocket connection and subscribes to order book updates. It allows the user to:
- **Specify an instrument** for real-time order book streaming.
- **Receive bid-ask updates** at 100ms intervals.
- **Process and log bid-ask price levels** for market analysis.
- **Gracefully handle disconnections** and errors.

---

## **2. Connect to Deribit**

This function initializes a secure WebSocket connection to Deribit Testnet. It:
- **Resolves the WebSocket endpoint** for `test.deribit.com`.
- **Performs an SSL handshake** for secure communication.
- **Establishes a connection** to receive market data.

---

## **3. Subscribe to Order Book**

This function sends a subscription request to receive live order book updates. It:
- **Constructs a JSON request** to subscribe to `book.{instrument}.agg2`.
- **Writes the request** to the WebSocket stream.
- **Confirms the subscription** upon a successful response.

---

## **4. Receive Order Book Data**

This function listens for real-time order book updates. It:
- **Reads incoming WebSocket messages** and processes market data.
- **Extracts bid and ask prices** and their corresponding volumes.
- **Limits the displayed order book levels** based on `max_levels`.
- **Logs the bid-ask data** for monitoring and strategy execution.

---

## **5. Disconnect from Deribit**

This function safely closes the WebSocket connection. It:
- **Sends a close request** to the WebSocket stream.
- **Handles unexpected disconnections** and logs any errors.
- **Ensures clean termination** of the connection.

---
---

# **Utilities Module**

## **Overview**  
The `utilities` module provides essential helper functions for authentication, currency selection, instrument retrieval, and fetching open orders on Deribit Testnet. It enables smooth integration with Deribit’s API.

This module utilizes `CURL` for API communication, `Boost.JSON` for response parsing, and proper error handling and logging via `spdlog`.

---

## **1. Authentication**  

Handles user authentication by requesting an access token from Deribit Testnet. It:  
- Accepts `client_id` and `client_secret`  
- Constructs an authentication request  
- Sends the request using `CURL`  
- Extracts and returns the `access_token`  

---

## **2. Get Currency**  

Fetches supported currencies available on Deribit Testnet. It:
- Queries `public/get_currencies` API for available currencies 
- Displays the list to the user  
- Allows the user to `select a currency`    

---

## **3. Get Instrument**  

Retrieves tradable instruments for a selected currency. It:  
- Takes `currency` as input  
- Prompts the user to choose an `instrument type` (Spot, Future, or Option)  
- Queries `public/get_instruments` API for available instruments  
- Displays the list and allows `user selection`  

---

## **4. Get Open Orders**  

Retrieves a list of currently open orders from Deribit Testnet. It:  
- Requires an authenticated `access_token`  
- Queries `private/get_open_orders` API for Open Orders
- Parses the JSON response to extract order details  
- Returns a list of `OrderInfo` objects (order ID, instrument, price, amount) 
