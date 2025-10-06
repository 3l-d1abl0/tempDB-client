#include "Cli.hpp"
#include <string>
#include <sstream>
#include <stdexcept>

namespace tempdb {

Cli::ParseResult Cli::parseArguments(int argc, char* argv[]) {
    
    std::stringstream ss;
    ss << "Usage: " << argv[0] << " -h <host-ip> -p <port>" << '\n';
    ss << "OR: " << argv[0] << " -host <host-ip> -port <port>" << '\n';

    if (argc != 5) {
        return ParseResult(false, "", 0, ss.str());
    }

    std::string hostFlag = argv[1];
    std::string hostValue = argv[2];
    std::string portFlag = argv[3];
    std::string portValue = argv[4];

    // Check for valid host flags
    if (hostFlag != "-h" && hostFlag != "-host") {
        ss << "Error: Invalid host flag '" << hostFlag << "'. Expected -h or -host.";
        return ParseResult(false, "", 0, ss.str());
    }

    // Check for valid port flags
    if (portFlag != "-p" && portFlag != "-port") {
        ss << "Error: Invalid port flag '" << portFlag << "'. Expected -p or -port.";
        return ParseResult(false, "", 0, ss.str());
    }

    try {
        validateHost(hostValue);
        int port = validatePort(portValue);

        return ParseResult(true, hostValue, port);
    } catch (const std::invalid_argument& e) {
        return ParseResult(false, "", 0, e.what());
    }
}

int Cli::validatePort(const std::string& portString) {

    if (portString.empty()) {
        throw std::invalid_argument("Port number cannot be empty");
    }

    // Check if all characters are digits
    for (char c : portString) {
        if (!std::isdigit(c)) {
            throw std::invalid_argument("Port number must contain only digits");
        }
    }

    int port = std::stoi(portString);

    if (port <= 0 || port > 65535) {
        throw std::invalid_argument("Port number must be between 1 and 65535");
    }

    return port;
}

void Cli::validateHost(const std::string& host) {
    
    if (host.empty()) {
        throw std::invalid_argument("Host cannot be empty");
    }

    for (char c : host) {

        // Basic validation - check for obviously invalid characters
        if (c < 32 || c > 126) {
            throw std::invalid_argument("Host contains invalid characters");
        }
    

        // Check for valid hostname/IP format (simplified)
        // Allow alphanumeric, dots, hyphens, and colons (for IPv6)
        if (!std::isalnum(c) && c != '.' && c != '-' && c != ':') {
            throw std::invalid_argument("Host contains invalid characters. Only alphanumeric, dots, hyphens, and colons are allowed");
        }
    }
}


} // namespace tempdb