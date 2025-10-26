#include "Client.hpp"

#include <codecvt>
#include <iostream>
#include <ostream>


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

        if (!sendCommand(input)) {
            return false;
        }

        if (!receiveResponse()) {
            return false;
        }

        return true;
    }

    bool Client::sendCommand(const std::string& command) {
        try {

            std::string commandWithNewline = command + "\n";
            std::cout<<"Sending Command ..."<<commandWithNewline<<std::endl;
            network_->sendData(commandWithNewline);
            return true;
        } catch (const std::runtime_error& e) {
            std::cerr << "Error sending command: " << e.what() << std::endl;
            connected_ = false;
            return false;
        }
    }

    bool Client::receiveResponse() {
        const size_t bufferSize = 1024;
        char buffer[bufferSize];

        std::cout<<"RECIEVING: "<<std::endl;
        try {

            int bytesReceived = network_->receiveData(buffer, bufferSize);
            if (bytesReceived == 0) {
                std::cerr << "Server closed the connection" << std::endl;
                connected_ = false;
                return false;
            }

            std::cout<<"RECIEVED: "<<std::endl;
            std::cout << buffer << std::endl;

            return true;
        } catch (const std::runtime_error& e) {
            std::cerr << "Error receiving response: " << e.what() << std::endl;
            connected_ = false;
            return false;
        }
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
