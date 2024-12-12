#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <unordered_map>
#include <mutex>
#include <condition_variable>

using namespace std;
using json = nlohmann::json;

using websocketpp::connection_hdl;
typedef websocketpp::client<websocketpp::config::asio_client> client;

// Mutex for thread-safe data access
std::mutex data_mutex;
std::condition_variable data_cv;
std::unordered_map<std::string, json> orderbook_data;

void on_message(websocketpp::connection_hdl hdl, client::message_ptr msg) {
    std::lock_guard<std::mutex> lock(data_mutex);

    // Parse the incoming message
    json message = json::parse(msg->get_payload());

    // Check if it contains order book updates
    if (message.contains("params") && message["params"].contains("channel")) {
        std::string channel = message["params"]["channel"];
        orderbook_data[channel] = message["params"]["data"];
        data_cv.notify_all();
    }
}

void subscribe_to_channels(client &c, websocketpp::connection_hdl hdl) {
    json subscription_message = {
        {"jsonrpc", "2.0"},
        {"id", 42},
        {"method", "public/subscribe"},
        {"params", {
            {"channels", {
                "book.BTC-PERPETUAL.raw",
                "book.ETH-PERPETUAL.raw"
            }}
        }}
    };
///// future scope i will store the data in a vector<string> symbols to give the user the lsit of subscribed symbols
// while ensuring low latency
    std::string message = subscription_message.dump();
    c.send(hdl, message, websocketpp::frame::opcode::text);
}

void websocket_thread() {
    client c;

    try {
        // Initialize WebSocket client
        c.init_asio();

        c.set_message_handler(&on_message);
        c.set_open_handler([&c](websocketpp::connection_hdl hdl) {
            std::cout << "Connected to Deribit test WebSocket!" << std::endl;
            subscribe_to_channels(c, hdl);
        });

        c.set_fail_handler([](websocketpp::connection_hdl) {
            std::cerr << "Connection failed!" << std::endl;
        });

        c.set_close_handler([](websocketpp::connection_hdl) {
            std::cerr << "Connection closed!" << std::endl;
        });

        std::string uri = "ws://test.deribit.com/ws/api/v2";
        websocketpp::lib::error_code ec;
        client::connection_ptr con = c.get_connection(uri, ec);

        if (ec) {
            std::cerr << "Could not create connection because: " << ec.message() << std::endl;
            return;
        }
	else {
    		std::cout << "Successful connection" << std::endl;
    		c.connect(con);
	}
        //c.connect(con);

        //c.run();

    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception!" << std::endl;
    }
}

void stream_orderbook_updates() {
    while (true) {
        std::unique_lock<std::mutex> lock(data_mutex);
        data_cv.wait(lock, [] { return !orderbook_data.empty(); });

        for (const auto &[channel, data] : orderbook_data) {
            std::cout << "Channel: " << channel << std::endl;
            std::cout << "Data: " << data.dump(4) << std::endl;
        }

        // Clear the processed data to avoid duplicate printing
        orderbook_data.clear();
    }
}

int main() {
    std::thread ws_thread(websocket_thread);
    std::thread stream_thread(stream_orderbook_updates);

    ws_thread.join();
    stream_thread.join();

    return 0;
}

