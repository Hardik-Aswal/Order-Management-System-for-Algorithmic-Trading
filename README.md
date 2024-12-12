# Order-Management-System-for-Algorithmic-Trading
This Project is a Low Latency Order Management System to Trade on https://test.deribit.com/ implemented in C++.

Real-Time Market Data Fetcher using WebSocket
Overview
This project demonstrates how to connect to a WebSocket and subscribe to order book updates for BTC and ETH perpetual contracts from Deribit. The data is parsed and displayed in real time using the websocketpp library and nlohmann/json for JSON parsing.

Prerequisites
Before you begin, ensure you have the following installed:

C++ Compiler: Either GCC or Clang should be installed on your system./
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
