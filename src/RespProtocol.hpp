#pragma once

#include <string>
#include <vector>

namespace tempdb {

/**
 * @brief Handles Redis Serialization Protocol (RESP) encoding and decoding
 * for tempDB
 * This class provides functionality to parse RESP-formatted messages and
 * convert them to/from C++ equivalent data structures.
 */
class RespProtocol {
public:

    /**
    * @brief Types of RESP protocol messages supported
    */
    enum class ResponseType {
        SIMPLE_STRING,  // +<string>\r\n
        ERROR,          // -<error message>\r\n
        INTEGER,        // :<number>\r\n
        BULK_STRING,    // $<length>\r\n<data>\r\n
        ARRAY,          // *<count>\r\n<elements...>
        UNKNOWN         // Unrecognized message type
    };

    /**
     * @brief Structure to hold a parsed RESP response
     */
    struct Response {
        ResponseType type;
        std::string value;
        std::vector<std::string> arrayElements;

        Response(ResponseType t = ResponseType::UNKNOWN, const std::string& v = "", const std::vector<std::string>& arr = {})
            : type(t), value(v), arrayElements(arr) {}
    };

    /**
     * @brief Splits input string into tokens, handling quoted strings
     * @param input The input string to split
     * @return Vector of token strings
     */
    static std::vector<std::string> splitInput(const std::string& input);

    /**
     * @brief Encodes a vector of strings as a RESP array
     * @param tokens Strings to encode
     * @return RESP-formatted string
     */
    static std::string encodeArray(const std::vector<std::string>& tokens);

    /**
     * @brief Parses a RESP-formatted response
     * @param response The raw RESP message
     * @return Parsed Response object
     */
    static Response parseResponse(const std::string& response);

    /**
     * @brief Converts a RESP representation to human-readable string
     * @param response The Response to convert
     * @return Human-readable string representation
     */
    static std::string humanize(const Response& response);

private:
    // Private methods for parsing RESP types
    static Response parseSimpleString(const std::string& response, size_t& pos);
    static Response parseError(const std::string& response, size_t& pos);
    static Response parseInteger(const std::string& response, size_t& pos);
    static Response parseBulkString(const std::string& response, size_t& pos);
    static Response parseArray(const std::string& response, size_t& pos);

};

} // namespace tempdb