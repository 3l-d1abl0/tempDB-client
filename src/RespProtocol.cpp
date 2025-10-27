#include "RespProtocol.hpp"

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <stdexcept>

namespace tempdb {

std::vector<std::string> RespProtocol::splitInput(const std::string& input) {
    std::vector<std::string> tokens;
    std::istringstream iss(input);
    std::string token;

    while (iss >> token) {
        tokens.push_back(token);
    }

    return tokens;
}


std::string RespProtocol::encodeArray(const std::vector<std::string>& tokens) {
    std::ostringstream resp;

    resp << "*" << tokens.size() << "\r\n";
    for (const auto& t : tokens) {
        resp << "$" << t.size() << "\r\n" << t << "\r\n";
    }
    return resp.str();
}

RespProtocol::Response RespProtocol::parseResponse(const std::string& response) {

    
    if (response.empty()) {
        return Response(ResponseType::UNKNOWN, "(empty response)");
    }

    size_t pos = 0;
    char responseType = response[pos];

    switch (responseType) {
        case '+':
            return parseSimpleString(response, pos);
        case '-':
            return parseError(response, pos);
        case ':':
            return parseInteger(response, pos);
        case '$':
            return parseBulkString(response, pos);
        case '*':
            return parseArray(response, pos);
        default:
            return Response(ResponseType::UNKNOWN, "[Unrecognized response]");
    }
}

RespProtocol::Response RespProtocol::parseSimpleString(const std::string& response, size_t& pos) {

    size_t end = response.find("\r\n", pos + 1);
    if (end == std::string::npos) {
        throw std::runtime_error("Invalid RESP simple string format");
    }

    std::string value = response.substr(pos + 1, end - pos - 1);
    pos = end + 1;
    return Response(ResponseType::SIMPLE_STRING, value);
}

RespProtocol::Response RespProtocol::parseError(const std::string& response, size_t& pos) {
    
    //Same as Simple String
    size_t end = response.find("\r\n", pos + 1);
    if (end == std::string::npos) {
        throw std::runtime_error("Invalid RESP error format");
    }

    std::string value = response.substr(pos + 1, end - pos - 1);
    pos = end + 1;
    return Response(ResponseType::ERROR, value);
}

RespProtocol::Response RespProtocol::parseInteger(const std::string& response, size_t& pos) {
    
    //Same as Simple String
    size_t end = response.find("\r\n", pos + 1);
    if (end == std::string::npos) {
        throw std::runtime_error("Invalid RESP integer format");
    }

    std::string value = response.substr(pos + 1, end - pos - 1);
    pos = end + 1;
    return Response(ResponseType::INTEGER, value);
}

RespProtocol::Response RespProtocol::parseBulkString(const std::string& response, size_t& pos) {
    
    // Find the end of the length line
    size_t rn = response.find("\r\n", pos + 1);
    if (rn == std::string::npos) {
        throw std::runtime_error("Invalid RESP bulk string format");
    }

    int len = std::stoi(response.substr(pos + 1, rn - pos - 1));
    if (len == -1) {
        pos = rn + 1;
        return Response(ResponseType::BULK_STRING, "");
    }

    // Calculate start position of the actual string
    size_t start = rn + 2;
    if (start + len > response.size()) {
        throw std::runtime_error("RESP bulk string length mismatch");
    }

    std::string value = response.substr(start, len);
    pos = start + len + 1; // Skip the string + final \r\n
    return Response(ResponseType::BULK_STRING, value);
}

RespProtocol::Response RespProtocol::parseArray(const std::string& response, size_t& pos) {
    
    // Find the end of the array length line
    size_t rn = response.find("\r\n", pos + 1);
    if (rn == std::string::npos) {
        throw std::runtime_error("Invalid RESP array format");
    }

    int numElements = std::stoi(response.substr(pos + 1, rn - pos - 1));

    if (numElements == -1) {
        pos = rn + 1;
        return Response(ResponseType::ARRAY, "");
    }

    std::vector<std::string> elements;
    size_t currentPos = rn + 2;

    for (int i = 0; i < numElements; ++i) {
        if (currentPos >= response.size()) {
            throw std::runtime_error("RESP array incomplete");
        }

        Response element = parseResponse(response.substr(currentPos));
        if (element.type == ResponseType::BULK_STRING) {
            elements.push_back(element.value);
        } else {
            elements.push_back(humanize(element));
        }

        currentPos += response.size(); 
        break; 
    }

    pos = response.size() - 1;
    return Response(ResponseType::ARRAY, "", elements);
}


std::string RespProtocol::humanize(const Response& response) {
    
    switch (response.type) {
        case ResponseType::SIMPLE_STRING:
            return response.value;
        case ResponseType::ERROR:
            return "Error: " + response.value;
        case ResponseType::INTEGER:
            return response.value;
        case ResponseType::BULK_STRING:
            return response.value.empty() ? "(nil)" : response.value;
        case ResponseType::ARRAY:
            if (response.arrayElements.empty()) {
                return "(empty array)";
            }
            {
                std::ostringstream oss;
                for (size_t i = 0; i < response.arrayElements.size(); ++i) {
                    if (i > 0)
                        oss << " ";
                    oss << response.arrayElements[i];
                }
                return oss.str();
            }
        case ResponseType::UNKNOWN:
        default:
            return response.value;
    }
}

} //namespace tempdb