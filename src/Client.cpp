#include "Client.hpp"

#include <iostream>


namespace tempdb {

    Client::Client(const std::string& host, int port)
        : network_(std::make_unique<Network>(host, port)), host_(host), port_(port), connected_(true) {
    }

    int Client::run() {
        
        std::cout << "Interactive tempDB client session started." << std::endl;
        std::cout << "Type 'exit' or press Ctrl+D to quit." << std::endl;
        std::cout << std::endl;

        std::string input;

        while (connected_) {
            displayPrompt();

            if (!std::getline(std::cin, input)) {
                // Handle Ctrl+D (EOF)
                std::cout << std::endl;
                break;
            }

            if (!processCommand(input)) {
                break;
            }
        }

        if (connected_) {
            handleDisconnection();
        }

        return 0;
    }

    bool Client::processCommand(const std::string& input) {
        // Exit if user inputs 'exit' or empty string (Ctrl+D)
        if (input.empty() || input == "exit") {
            return false;
        }

        std::cout << input<<std::endl;

        return true;
    }

    void Client::displayPrompt() {
        std::cout << host_ << ":" << port_ << " > ";
        std::cout.flush();
    }

    void Client::handleDisconnection() {
        std::cout << "Disconnecting from server..." << std::endl;
        connected_ = false;
    }

} // namespace tempdb
