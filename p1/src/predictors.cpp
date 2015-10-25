#include "predictors.hpp"

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <cstdlib>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cout << "Invalid command format: ./predictors [input trace] [output file]\n";
        exit(EXIT_FAILURE);
    }

    std::ifstream input_trace(argv[1]);
    std::string line, behavior;
    unsigned long long addr;
    while(getline(input_trace, line)) {
        // Now we have to parse the line into it's two pieces
        std::stringstream s(line);
        s >> std::hex >> addr >> behavior;
        // Now we can output it
        std::cout << addr;
        if(behavior == "T") {
            std::cout << " -> taken" << std::endl;
        }else {
            std::cout << " -> not taken" << std::endl;
        }
    }

    input_trace.close();

    std::ofstream output(argv[2]);
    output.close();
}
