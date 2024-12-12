#include <iostream>
#include <chrono>
#include <string>
#include <curl/curl.h>
#include "include/json.hpp"

using json = nlohmann::json;
using namespace std;
// Function to handle the response from the cURL request
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// General function to send a cURL request with optional access token
std::string sendRequest(const std::string& url, const json& payload, const std::string& accessToken = "") {
    std::string readBuffer;
    CURL* curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);  // Set the HTTP method to POST

        // Set the request payload
        std::string jsonStr = payload.dump();
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonStr.c_str());

        // Set headers, including Authorization if accessToken is provided
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        if (!accessToken.empty()) {
            headers = curl_slist_append(headers, ("Authorization: Bearer " + accessToken).c_str());
        }
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set up the write callback to capture the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // Perform the request
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "Request failed: " << curl_easy_strerror(res) << std::endl;
        }

        // Free Resources
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    return readBuffer;
}

// Function to get the access token
std::string getAccessToken(const std::string& clientId, const std::string& clientSecret) {
    json payload = {
        {"id", 0},
        {"method", "public/auth"},
        {"params", {
            {"grant_type", "client_credentials"},
            {"scope", "session:apiconsole-c5i26ds6dsr expires:2592000"},
            {"client_id", clientId},
            {"client_secret", clientSecret}
        }},
        {"jsonrpc", "2.0"}
    };

    std::string response = sendRequest("https://test.deribit.com/api/v2/public/auth", payload);
    auto responseJson = json::parse(response);

    // Retrieve the access token from the response
    if (responseJson.contains("result") && responseJson["result"].contains("access_token")) {
        return responseJson["result"]["access_token"];
    } else {
        std::cerr << "Failed to retrieve access token." << std::endl;
        return "";
    }
}
// Function to retrieve all active instruments for a specific currency
void getActiveInstruments(const std::string& currency) {
    json payload = {
        {"jsonrpc", "2.0"},
        {"method", "public/get_instruments"},
        {"params", {
            {"currency", currency},
            {"kind", "future"},  // Specify the type of instruments, e.g., "future", "option"
            {"expired", false}   // Only fetch active instruments
        }},
        {"id", 30}
    };

    std::string response = sendRequest("https://test.deribit.com/api/v2/public/get_instruments", payload);

    auto responseJson = json::parse(response);

    if (responseJson.contains("result")) {
        std::cout << "Available Instruments for " << currency << ":\n\n";

        for (const auto& instrument : responseJson["result"]) {
            std::cout << "Instrument Name: " << instrument["instrument_name"] 
                      << ", Min Trade Amount: " << instrument["min_trade_amount"]
                      << ", Tick Size: " << instrument["tick_size"] 
                      << ", Contract Size: " << instrument["contract_size"] 
                      << "\n";
        }
    } else {
        std::cerr << "Error: Could not retrieve instruments." << std::endl;
    }
}


// Function to place an order
void placeOrder(const std::string& price, const std::string& accessToken, const std::string& amount, const std::string& instrument) {
    json payload = {
        {"jsonrpc", "2.0"},
        {"method", "private/buy"},
        {"params", {
            {"instrument_name", instrument},
            {"type", "limit"},
            {"price", price},
            {"amount", amount}
        }},
        {"id", 1}
    };

    std::string response = sendRequest("https://test.deribit.com/api/v2/private/buy", payload, accessToken);
    auto responseJson = json::parse(response);

    if (responseJson.contains("result")) {
        auto order = responseJson["result"]["order"];
        std::string orderId = order["order_id"];
        double orderPrice = order["price"];
        double orderAmount = order["amount"];

        std::cout << "Order placed successfully!\n";
        std::cout << "Order ID: " << orderId << ", Price: " << orderPrice << ", Amount: " << orderAmount << "\n";
        std::cout << "Order State: " << order["order_state"] << "\n";
    } else {
        std::cerr << "Error placing order: "
                << responseJson["error"]["message"]
                << " (" << responseJson["error"]["code"] << ")"
                << std::endl;
    }

}

// Function to cancel an order
void cancelOrder(const std::string& accessToken, const std::string& orderID) {
    json payload = {
        {"jsonrpc", "2.0"},
        {"method", "private/cancel"},
        {"params", {{"order_id", orderID}}},
        {"id", 6}
    };

    std::string response = sendRequest("https://test.deribit.com/api/v2/private/cancel", payload,accessToken);
    std::cout << "Cancel Order Response: " << response << std::endl;
}

// Function to modify an order
void modifyOrder(const std::string& accessToken, const std::string& orderID, int amount, double price) {
    json payload = {
        {"jsonrpc", "2.0"},
        {"method", "private/edit"},
        {"params", {
            {"order_id", orderID},
            {"amount", amount},
            {"price", price}
        }},
        {"id", 11}
    };

    std::string response = sendRequest("https://test.deribit.com/api/v2/private/edit", payload,accessToken);
    std::cout << "Modify Order Response: " << response << std::endl;
}

// Function to retrieve the order book
void getOrderBook(const std::string& accessToken, const std::string& instrument) {
    json payload = {
        {"jsonrpc", "2.0"},
        {"method", "public/get_order_book"},
        {"params", {{"instrument_name", instrument}}},
        {"id", 15}
    };

    std::string response = sendRequest("https://test.deribit.com/api/v2/public/get_order_book",payload, accessToken);

    auto responseJson = json::parse(response);

      std::cout << "Order Book for " << instrument << ":\n\n";
            
            // Printing best bid and ask
            std::cout << "Best Bid Price: " << responseJson["result"]["best_bid_price"] << ", Amount: " << responseJson["result"]["best_bid_amount"] << '\n';
            std::cout << "Best Ask Price: " << responseJson["result"]["best_ask_price"] << ", Amount: " << responseJson["result"]["best_ask_amount"] << '\n';

            // Printing bids and asks in detail
            std::cout << "Asks:\n";
            for (const auto& ask : responseJson["result"]["asks"]) {
                std::cout << "Price: " << ask[0] << ", Amount: " << ask[1] << '\n';
            }

            std::cout << "\nBids:\n";
            for (const auto& bid : responseJson["result"]["bids"]) {
                std::cout << "Price: " << bid[0] << ", Amount: " << bid[1] << '\n';
            }

            // Additional information
            std::cout << "\nMark Price: " << responseJson["result"]["mark_price"] << '\n';
            std::cout << "Open Interest: " << responseJson["result"]["open_interest"] << '\n';
            std::cout << "Timestamp: " << responseJson["result"]["timestamp"] << '\n';
}

// Function to get position details of a specific instrument
// Function to get and print all open positions
void getOpenPositions(const std::string& accessToken, const std::string& currency) {
    json payload = {
        {"jsonrpc", "2.0"},
        {"method", "private/get_positions"},
        {"params", {
            {"currency", currency},
            {"kind", "future"} // Filter for futures; change to "option" if needed
        }},
        {"id", 35}
    };

    std::string response = sendRequest("https://test.deribit.com/api/v2/private/get_positions", payload, accessToken);
    auto responseJson = json::parse(response);

    if (responseJson.contains("result") && responseJson["result"].is_array()) {
        const auto& positions = responseJson["result"];

        if (positions.empty()) {
            std::cout << "No open positions for " << currency << ".\n";
        } else {
            std::cout << "Open Positions for " << currency << ":\n\n";
            for (const auto& position : positions) {
                std::cout << "Instrument: " << position["instrument_name"] << '\n';
                std::cout << "Size: " << position["size"] << '\n';
                std::cout << "Direction: " << position["direction"] << '\n';
                std::cout << "Average Price: " << position["average_price"] << '\n';
                std::cout << "Floating Profit/Loss: " << position["floating_profit_loss"] << '\n';
                std::cout << "Total Profit/Loss: " << position["total_profit_loss"] << '\n';
                std::cout << "Mark Price: " << position["mark_price"] << '\n';
                std::cout << "Liquidation Price: " << position["estimated_liquidation_price"] << '\n';
                std::cout << "---------------------------------\n";
            }
        }
    } else {
        std::cerr << "Error: Could not retrieve open positions." << std::endl;
    }
}


// Function to print all open orders with instrument, order ID, price, and amount
void getOpenOrders(const std::string& accessToken) {
    json payload = {
        {"jsonrpc", "2.0"},
        {"method", "private/get_open_orders"},
        {"params", {{"kind", "future"}, {"type", "limit"}}},
        {"id", 25}
    };

    std::string response = sendRequest("https://test.deribit.com/api/v2/private/get_open_orders", payload, accessToken);
    auto responseJson = json::parse(response);

    // Check if the response contains the "result" array
    if (responseJson.contains("result")) {
        std::cout << "Open Orders:\n\n";
        for (const auto& order : responseJson["result"]) {
            std::string instrument = order["instrument_name"];
            std::string orderId = order["order_id"];
            double price = order["price"];
            double amount = order["amount"];

            std::cout << "Instrument: " << instrument << ", Order ID: " << orderId
                      << ", Price: " << price << ", Amount: " << amount << '\n';
        }
    } else {
        std::cerr << "Error: Could not retrieve open orders." << std::endl;
    }
}

int main() {
    // Replace with your actual client credentials
    std::string clientId = "F6J_KY0G";
    std::string clientSecret = "Gm43FZJ4FJI0mMWjAZofJrS8vnCgdhijotDwyAzVUdo";

    // Retrieve the access token
    std::string accessToken = getAccessToken(clientId, clientSecret);

    if (!accessToken.empty()) {
        int toggle;

            do{
            cout<<endl;
            cout<<"Please select the task to perform : "<<endl;
            cout<<"1 - Place Order"<<endl;
            cout<<"2 - Cancel Order"<<endl;
            cout<<"3 - Modify Order"<<endl;
            cout<<"4 - Get Order Book"<<endl;
            cout<<"5 - View Current Positions"<<endl;
            cout<<"6 - View Active Instruments available to trade with all relevant information: "<<endl;
	    cout<<"7 - To Exit"<<endl;
            cout<<"Please enter your choice from (1-7)"<<endl;
            cin>>toggle;

	

            switch(toggle){

                case 1: {
                    cout<<"Let's Proceed with placing the order...."<<endl;
                    int n;
                    cout<<"Please enter the number of orders you want to place :"<<endl;
                    cin>>n;
                    for(int i=1;i<=n;i++){
                        // ask the user for price, amount and instrument
                        string price;
                        string amount;
                        string instrument;
                        cout<<"Please enter the price for Order Number ("<<i<<") :"<<endl;
                        cin>>price;
                        

                        cout<<"Please enter the amount for Order Number ("<<i<<") :"<<endl;
                        cin>>amount;

                        cout<<"Please enter the instrument for order Number ("<<i<<")"<<" to Trade: "<<endl;
                        cin>>instrument;
                        auto start = chrono::high_resolution_clock::now();
                        placeOrder(price,accessToken,amount,instrument);
			auto end = chrono::high_resolution_clock::now();
			chrono::duration<double,micro>latency = end-start;
			cout<<"Order placement latency is :"<<latency.count()<<" microseconds"<<endl;
                    }
			break;
                }

                case 2:{
                    string orderId;
                    cout<<"Order Id to cancel :"<<endl;
                    cin>>orderId;
                    cancelOrder(accessToken,orderId);
			break;
                }
                case 3:{
                    string orderId;
                    int amount;
                    double price;
                    cout<<"Enter the Order Id to Modify :"<<endl;
                    cin>>orderId;
                    cout<<"Enter the new amount :"<<endl;
                    cin>>amount;
                    cout<<"Enter the modified price :"<<endl;
                    cin>>price;
                    modifyOrder(accessToken,orderId,amount,price);
			break;
                }
                case 4:{
                    string instrument;
                    cout<<"Enter the instrument to view Order Book: "<<endl;
                    cin>>instrument;

                    getOrderBook(accessToken,instrument);
			break;
                }
                case 5:{
                    std::string currency;
                    std::cout << "Enter the currency to fetch open positions (e.g., BTC, ETH): ";
                    std::cin >> currency;

                    // Fetch and display open positions for the specified currency
                    getOpenPositions(accessToken, currency);
			break;
                }
                case 6:{
                    std::string currency;
                    std::cout << "Enter the currency to fetch active instruments (e.g., BTC, ETH): ";
                    std::cin >> currency;

                    // Fetch and display active instruments for the specified currency
                    getActiveInstruments(currency);
			break;
                }
		case 7:{
			cout<<"Exiting the Process"<<endl;
			return 0;// exit status
		}

		default :{
			cout<<"Invalid Input"<<endl;
		}
		}
            } while(toggle!=7);

    } else {
        std::cerr << "Unable to obtain access token, aborting operations." << std::endl;
    }

    return 0;
}

