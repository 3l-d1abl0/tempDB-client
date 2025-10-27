#pragma once

#include <string>
#include <memory>
#include <stdexcept>

struct addrinfo;

namespace tempdb {

    /**
    * @brief Network connection manager for tempDB client
    *
    * Handles socket creation, connection, and communication with the server.
    * Uses RAII principles for automatic resource management.
    */
    class Network {
    public:

        /**
        * @brief Constructor - creates socket and establishes connection
        * @param host Server hostname or IP address
        * @param port Server port number
        * @throws std::runtime_error if connection fails
        */
        explicit Network(const std::string& host, int port);

        /**
        * @brief Destructor - closes socket and cleans up resources
        */
        ~Network();

        ssize_t sendData(const std::string& data);

        int receiveData(char* buffer, size_t bufferSize);

    private:

        /**
        * @brief Create and connect socket
        * @param addr Address information for connection
        * @throws std::runtime_error if socket creation or connection fails
        */
        void createSocket(addrinfo* addr);

        std::string host_;  ///< Server hostname
        int port_;          ///< Server port
        int sock_;          ///< Socket file descriptor
        bool connected_;    ///< Connection status
    };

} // namespace tempdb
