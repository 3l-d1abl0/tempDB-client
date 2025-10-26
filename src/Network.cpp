#include "Network.hpp"

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <stdexcept>

namespace tempdb {

    Network::Network(const std::string& host, int port)
        : host_(host), port_(port), sock_(-1), connected_(false) {

        if (port <= 0 || port > 65535) {
            throw std::runtime_error("Invalid port number: " + std::to_string(port));
        }

        std::cout << "Resolving hostname..." << std::endl;
        std::string port_str = std::to_string(port_);

        // Resolve the hostname or IP address
        struct addrinfo hints = {}, *res;
        std::memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;       // IPv4, AF_UNSPEC for IPv6
        hints.ai_socktype = SOCK_STREAM; // TCP

        if (getaddrinfo(host_.c_str(), port_str.c_str(), &hints, &res) != 0) {
            throw std::runtime_error("Error: Could not resolve hostname: " + host_);
        }

        //Print the resolved address
        for (struct addrinfo* p = res; p != nullptr; p = p->ai_next) {
            // Print details about each address
            std::cout << "Address Family: " << p->ai_family << std::endl;
            std::cout << "Socket Type: " << p->ai_socktype << std::endl;
            std::cout << "Protocol: " << p->ai_protocol << std::endl;
            std::cout << "------------------------------" << std::endl;
        }

        try {
            createSocket(res);
            freeaddrinfo(res);
            connected_ = true;
            std::cout << "Connected to " << host_ << ":" << port_ << " successfully!" << std::endl;
        } catch (...) {
            freeaddrinfo(res);
            throw;
        }
    }

    Network::~Network() {
        if (sock_ >= 0) {
            close(sock_);
        }
    }

    void Network::createSocket(addrinfo* addr) {
        std::cout << "Creating socket..." << std::endl;

        sock_ = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        if (sock_ < 0) {
            throw std::runtime_error("Error: Could not create socket");
        }

        std::cout << "Connecting to server..." << std::endl;
        if (connect(sock_, addr->ai_addr, addr->ai_addrlen) < 0) {
            close(sock_);
            sock_ = -1;
            throw std::runtime_error("Error: Connection failed");
        }
    }


    ssize_t Network::sendData(const std::string& data) {
        if (!connected_) {
            throw std::runtime_error("Not connected to server");
        }

        ssize_t sent_bytes = send(sock_, data.c_str(), data.size(), 0);
        if (sent_bytes < 0) {
            connected_ = false;
            throw std::runtime_error("Error: Connection lost while sending data");
        }

        return sent_bytes;
    }

    int Network::receiveData(char* buffer, size_t bufferSize) {
        if (!connected_) {
            throw std::runtime_error("Not connected to server");
        }

        int bytes_received = recv(sock_, buffer, bufferSize - 1, 0);
        if (bytes_received < 0) {
            connected_ = false;
            throw std::runtime_error("Error: Failed to receive data from server");
        } else if (bytes_received == 0) {
            connected_ = false;
            return 0; // Server closed connection
        }

        buffer[bytes_received] = '\0'; // Null-terminate
        return bytes_received;
    }

} // namespace tempdb
