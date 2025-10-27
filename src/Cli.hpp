#pragma once

#include <string>

namespace tempdb {

/**
 * @brief Command line argument parser for tempDB client
 *
 * Handles parsing and validation of command line arguments with proper error handling.
 */
    class Cli {
    public:
        /**
        * @brief Parse result structure
        */
        struct ParseResult {
            bool success;
            std::string host;
            int port;
            std::string errorMessage;

            ParseResult(bool s, const std::string& h = "", int p = 0, const std::string& err = "")
                : success(s), host(h), port(p), errorMessage(err) {}
        };

        /**
        * @brief Parse command line arguments
        * @param argc Argument count
        * @param argv Argument vector
        * @return ParseResult with success status and parsed values
        */
        static ParseResult parseArguments(int argc, char* argv[]);

        /**
        * @brief Display usage information
        * @param programName Name of the program (argv[0])
        */
        static void displayUsage(const std::string& programName);

    private:
        /**
        * @brief Validate port number
        * @param portString Port number as string
        * @return Validated port number
        * @throws std::invalid_argument if port is invalid
        */
        static int validatePort(const std::string& portString);

        /**
        * @brief Validate hostname/IP address
        * @param host Hostname or IP address
        * @throws std::invalid_argument if host is invalid
        */
        static void validateHost(const std::string& host);
    };

} // namespace tempdb