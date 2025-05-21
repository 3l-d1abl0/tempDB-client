#include "Cli.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <utility>


//bin -h localhost -p port


std::pair<bool, std::vector<std::string>>  argumentsCheck(int argc, char* argv[]){

    
    std::pair<bool, std::vector<std::string>> arguments;
    
    std::stringstream ss;
    ss <<"Usage: "<< argv[0]<<" -h <host-ip> -p <port>"<<'\n';
    ss <<"OR: "<< argv[0]<<" -host <host-ip> -port <port>"<<'\n';
    std::string usageInfo = ss.str();

    // Parse command-line arguments
    if (argc != 5){
        std::cerr <<usageInfo;
        return {false, {}};
    }
    
    //Check for host or h
    if(std::string(argv[1]) != "-h" && std::string(argv[1]) != "-host"){
        std::cerr <<usageInfo;
        std::cerr << "Missing -h or -host flag"<<std::endl;
        return {false, {}};
    }

    if(std::string(argv[3]) != "-p" && std::string(argv[3]) != "-port") {
        std::cerr <<usageInfo;
        std::cerr << "Missing -p or -port flag"<<std::endl;
        return {false, {}};
    }

    return {true, {argv[2], argv[4]}};
}