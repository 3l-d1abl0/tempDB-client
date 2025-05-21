#include <iostream>
#include <vector>
#include <utility>
#include <string>

#include "Cli.h"

int main(int argc, char* argv[]) {

    std::pair<bool, std::vector<std::string>> args = argumentsCheck(argc, argv);
    
    std::cout<<args.first<<std::endl;
    for(std::string val: args.second){
        std::cout<<val<<std::endl;
    }

    return 0;
}