#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h> // For getaddrinfo
#include <unistd.h>
#include <cstdlib>
#include <vector>

int  argumentsCheck(int argc, char* argv[]){

    std::stringstream ss;
    ss <<"Usage: "<< argv[0]<<" -h <host-ip> -p <port>"<<'\n';
    ss <<"OR: "<< argv[0]<<" -host <host-ip> -port <port>"<<'\n';
    std::string usageInfo = ss.str();

    // Parse command-line arguments
    if (argc != 5){
        std::cerr <<usageInfo;
        return EXIT_FAILURE;
    }
    
    //Check for host or h
    if(std::string(argv[1]) != "-h" && std::string(argv[1]) != "-host"){
        std::cerr <<usageInfo;
        std::cerr << "Missing -h or -host flag"<<std::endl;
        return EXIT_FAILURE;
    }

    if(std::string(argv[3]) != "-p" && std::string(argv[3]) != "-port") {
        std::cerr <<usageInfo;
        std::cerr << "Missing -p or -port flag"<<std::endl;
        return EXIT_FAILURE;
    }

    return 0;
}

// Helper: Split string by spaces (handles quoted strings as single token)
std::vector<std::string> split_input(const std::string& input) {
    
    std::vector<std::string> tokens;
    std::istringstream iss(input);
    std::string token;
    
    while (iss >> token) {
        tokens.push_back(token);
    }
    
    return tokens;
}

// Helper: Encode vector<string> as RESP array
std::string encode_resp(const std::vector<std::string>& tokens) {
    std::ostringstream resp;
    resp << "*" << tokens.size() << "\r\n";
    for (const auto& t : tokens) {
        resp << "$" << t.size() << "\r\n" << t << "\r\n";
    }
    return resp.str();
}

// RESP response parser for +, -, :, and $ types
std::string parse_resp_response(const std::string& resp) {
    if (resp.empty()) return "(empty response)";
    if (resp[0] == '+') {
        // Simple String
        size_t end = resp.find("\r\n");
        return resp.substr(1, end - 1);
    } else if (resp[0] == '-') {
        // Error
        size_t end = resp.find("\r\n");
        return "Error: " + resp.substr(1, end - 1);
    } else if (resp[0] == ':') {
        // Integer
        size_t end = resp.find("\r\n");
        return resp.substr(1, end - 1);
    } else if (resp[0] == '$') {
        // Bulk String
        size_t rn = resp.find("\r\n");
        int len = std::stoi(resp.substr(1, rn - 1));
        if (len == -1) return "(nil)";
        size_t start = rn + 2;
        return resp.substr(start, len);
    } else {
        return "[Unrecognized response]";
    }
}

int main(int argc, char* argv[]) {

    //1. Check arguments
    int status = argumentsCheck(argc, argv);
    if(status==1)
        return EXIT_FAILURE;


    std::string host = argv[2];
    int port = std::atoi(argv[4]);
    
    //2. check for port
    if (port <= 0 || port > 65535) {
        std::cerr << "Invalid port number" << std::endl;
        return EXIT_FAILURE;
    }

    // Resolve the hostname or IP address
    struct addrinfo hints = {}, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP

    std::cout<<"Resolving hostname ..."<<std::endl;
    std::string service = std::to_string(port);
    if (getaddrinfo(host.c_str(), service.c_str(), &hints, &res) != 0) {
        std::cerr << "Error: Could not resolve hostname: " << host << std::endl;
        return EXIT_FAILURE;
    }
    //std::cout<<res->ai_addr<<" "<<res->ai_addrlen<<" "<<res->ai_family<<" "<<res->ai_socktype<<" "<<res->ai_protocol<<std::endl;

    std::cout<<"Creating socket ..."<<std::endl;
    // Create a TCP socket
    int sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock < 0) {
        std::cerr << "Error: Could not create socket" << std::endl;
        freeaddrinfo(res);
        return EXIT_FAILURE;
    }

    std::cout<<"Connecting to Server ..."<<std::endl;
    // Connect to the server
    if (connect(sock, res->ai_addr, res->ai_addrlen) < 0) {
        std::cerr << "Error: Connection failed" << std::endl;
        close(sock);
        freeaddrinfo(res);
        return EXIT_FAILURE;
    }

    freeaddrinfo(res); // Free the memory allocated by getaddrinfo

    std::cout << "Connected to " << host << ":" << port << " successfully!" << std::endl;

    // Main loop for sending commands and receiving responses
    std::string input;
    char buffer[1024] = {0};
    while (true) {
        std::cout << host << ":" << port << " > ";
        std::getline(std::cin, input);

        // Exit if user inputs 'exit' or presses Ctrl+D
        if (input.empty() || input == "exit") {
            break;
        }

        // Convert input to RESP format
        std::vector<std::string> tokens = split_input(input);
        if (tokens.empty()) continue;
        for(auto str: tokens)
            std::cout<<str<<" ";
        std::cout<<std::endl;
        std::string resp_cmd = encode_resp(tokens);
        std::cout<<resp_cmd<<std::endl;
        // Send the RESP command to the server
        ssize_t sent_bytes = send(sock, resp_cmd.c_str(), resp_cmd.size(), 0);
        if (sent_bytes < 0) {
            std::cerr << "Error: Connection lost while sending data" << std::endl;
            break;
        }

        // Read the response from the server
        int bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {  // Changed to check for both errors and disconnection
            if (bytes_received == 0) {
                std::cerr << "Error: Server closed the connection" << std::endl;
            } else {
                std::cerr << "Error: Failed to receive data from server" << std::endl;
            }
            break;
        }

        buffer[bytes_received] = '\0'; // Null-terminate the received data
        std::string resp(buffer);
        std::string human = parse_resp_response(resp);
        std::cout << human << std::endl;
    }

    // Close the socket and exit
    close(sock);
    return EXIT_SUCCESS;
}
