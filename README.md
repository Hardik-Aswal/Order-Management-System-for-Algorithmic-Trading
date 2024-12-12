# Order-Management-System-for-Algorithmic-Trading
This Project is a Low Latency Order Management System to Trade on https://test.deribit.com/ implemented in C++.

# Running main.cpp - Deribit API Client

This README provides instructions on how to run the main.cpp file, which is a command-line tool for interacting with the Deribit API.

Prerequisites

Environment: You need a Linux-based system with a working g++ compiler. The program uses cURL to send HTTP requests to the Deribit API.

# Dependencies:

g++ (GNU C++ compiler)

libcurl (for making HTTP requests)

nlohmann/json library (for JSON parsing and manipulation)

Install Dependencies

Install g++:

bash

Copy code

sudo apt update

sudo apt install g++

Install libcurl:

bash

Copy code

sudo apt install libcurl4-openssl-dev

Install nlohmann/json:

You can download and install the nlohmann/json library manually from its GitHub repository or use a package manager if available.

Alternatively, you can place the nlohmann/json header file directly in the appropriate include path. If not already installed, download it from: 

https://github.com/nlohmann/json.

Setup

Clone the repository (if not already done):

bash

Copy code

git clone https://github.com/username/repository.git

cd repository

Compile the code: Navigate to the directory where main.cpp is located and compile it using g++:

bash

Copy code

g++ -o deribit_client main.cpp -lcurl -std=c++11

Running the Program

Obtain API Credentials:

You will need a client ID and client secret from the Deribit API.

Sign up for an API key on the Deribit website and retrieve your credentials.

Run the Program:

bash

Copy code

./deribit_client

Use the interactive menu: The program will prompt you to enter your credentials and provide an interactive menu where you can choose:

Place, cancel, or modify orders

Fetch order book details

View active instruments

Retrieve open positions

Commands:

Follow the prompts to perform actions (e.g., entering the currency, price, amount, instrument, etc.).

Type 7 to exit the program.

# Notes

The program expects valid API credentials.

The getAccessToken, placeOrder, cancelOrder, modifyOrder, getOrderBook, and getOpenPositions functions handle various API requests.

Latency times for order operations are calculated and displayed as a measure of performance.

Make sure you have network access and the correct environment setup before running the program.

# Real-Time Market Data Fetcher using WebSocket

Overview

This project demonstrates how to connect to a WebSocket and subscribe to order book updates for BTC and ETH perpetual contracts from Deribit. The data is parsed and displayed in real time using the websocketpp library and nlohmann/json for JSON parsing.

Prerequisites
Before you begin, ensure you have the following installed:

C++ Compiler: Either GCC or Clang should be installed on your system.

CMake: Required for compiling the project.

WebSocket++: A C++ WebSocket library used for connecting and handling WebSocket messages.

nlohmann/json: A popular JSON library for C++.

You can install the required libraries using package managers:


Ubuntu/Debian:

bash

Copy code

sudo apt-get install cmake g++ libwebsocketpp-dev nlohmann-json3-dev

MacOS:

bash

Copy code

brew install cmake boost websocketpp

Windows: Use the appropriate installer for your C++ development environment or the vcpkg tool.

Setup

Clone the repository:

bash

Copy code

git clone https://github.com/Hardik-Aswal/Order-Management-System-for-Algorithmic-Trading.git

cd Order-Management-System-for-Algorithmic-Trading

Navigate to the source directory:

bash

Copy code

cd src

Compile the C++ program:

Use CMake to generate build files and compile the source:

bash

Copy code

mkdir build

cd build

cmake ..

make

Run the Program:

To execute the program, simply run:

bash

Copy code

./real-time-market-data

Running the Program

The program connects to the WebSocket server, subscribes to the BTC and ETH perpetual book channels, and listens for incoming updates. It prints the order book data in real-time to the console.

Output: The program continuously displays order book updates for the subscribed channels. The output format includes the channel name and the order book data formatted as a JSON string for readability.
Debugging and Issues

If you encounter issues:

Check if the WebSocket server URL (ws://test.deribit.com/ws/api/v2) is still valid. It might change over time.

Ensure network connectivity:

The WebSocket requires a stable internet connection.

Error handling:

Modify the program to include more error checks (like connection issues, JSON parsing failures, etc.).

Notes

The program runs indefinitely until you manually stop it.

The data fetching part (stream_orderbook_updates function) is designed to run in a separate thread and waits for new data before printing it.

This example is meant for educational purposes. For production use, consider handling connection re-establishment and error recovery more robustly.
