#include "predictors.hpp"

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <algorithm>

std::string yes_predicitor(const std::vector<Prediction>& preds) {
    int taken = count_if(preds.begin(), preds.end(),
            [](Prediction p){return p.taken;});
    return std::to_string(taken) + "," + std::to_string(preds.size()) + ";";
}

std::string no_predicitor(const std::vector<Prediction>& preds) {
    int taken = count_if(preds.begin(), preds.end(),
            [](Prediction p){return !p.taken;});
    return std::to_string(taken) + "," + std::to_string(preds.size()) + ";";
}

std::string bimodal_predicitor_1(const std::vector<Prediction>&) {
    return "";
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cout << "Invalid command format: ./predictors [input trace] [output file]\n";
        exit(EXIT_FAILURE);
    }

    std::ifstream input_trace(argv[1]);
    std::string line, behavior;
    unsigned long long addr;
    std::vector<Prediction> preds;
    while(getline(input_trace, line)) {
        // Now we have to parse the line into it's two pieces
        std::stringstream s(line);
        s >> std::hex >> addr >> behavior;
        // Now we can push it into the vector
        preds.push_back(Prediction(behavior, addr));
    }
    input_trace.close();

    std::ofstream output(argv[2]);

    output << yes_predicitor(preds) << "\n";
    output << no_predicitor(preds) << "\n";

    output.close();
}
