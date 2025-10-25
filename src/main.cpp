#include <iostream>
#include <cstdlib>

#include "Cli.hpp"
#include "Client.hpp"

int main(int argc, char* argv[]) {

    try {
        // Parse command line arguments
        auto argParseResult = tempdb::Cli::parseArguments(argc, argv);

        if (!argParseResult.success) {
            std::cerr << argParseResult.errorMessage << std::endl;
            tempdb::Cli::displayUsage(argv[0]);
            return EXIT_FAILURE;
        }

        std::cout << "Connecting to " << argParseResult.host << ":" << argParseResult.port << "..." << std::endl;

        // Create and run the client
        auto client = std::make_unique<tempdb::Client>(argParseResult.host, argParseResult.port);

        int exitCode = client->run();

        std::cout << "Client session ended." << std::endl;
        return exitCode;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return EXIT_FAILURE;
    }
}