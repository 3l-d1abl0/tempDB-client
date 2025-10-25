#pragma once

#include <string>
#include <memory>

#include "Network.hpp"

namespace tempdb {

    /**
    * @brief Main tempDB client class
    *
    * Coordinates network communication, RESP protocol handling, and user interaction.
    */
    class Client {
    public:
        /**
        * @brief Constructor
        * @param host Server hostname or IP address
        * @param port Server port number
        */
        explicit Client(const std::string& host, int port);

        /**
        * @brief Destructor
        */
        ~Client() = default;

        /**
        * @brief Run the interactive client session
        * @return Exit status (0 for success, non-zero for error)
        */
        int run();

    private:
        /**
        * @brief Process a single user command
        * @param input User input string
        * @return true to continue, false to exit
        */
        bool processCommand(const std::string& input);

        /**
        * @brief Display connection information prompt
        */
        void displayPrompt();

        /**
        * @brief Handle disconnection and cleanup
        */
        void handleDisconnection();

        std::unique_ptr<Network> network_;          ///< Network connection handler
        std::string host_;                         ///< Server hostname
        int port_;                                 ///< Server port
        bool connected_;                           ///< Connection status
    };

} // namespace tempdb
