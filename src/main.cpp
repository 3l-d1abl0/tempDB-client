#include <iostream>
#include <cstdlib>

#include "Cli.hpp"

int main(int argc, char* argv[]) {

    try {
        // Parse command line arguments
        auto argParseResult = tempdb::Cli::parseArguments(argc, argv);

        if (!argParseResult.success) {
            std::cerr << argParseResult.errorMessage << std::endl;
            return EXIT_FAILURE;
        }

        std::cout << "Connecting to " << argParseResult.host << ":" << argParseResult.port << "..." << std::endl;

        return EXIT_SUCCESS;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return EXIT_FAILURE;
    }
}