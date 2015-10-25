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

std::string bimodal_predicitor_1(const std::vector<Prediction>& preds) {
    int table_sizes[] = {16, 32, 128, 256, 512, 1024, 2048};
    std::ostringstream ss;
    for (int i = 0; i < 7; i++) {
        // Table for 1-bit predictor 
        int size = table_sizes[i];
        std::vector<bool> table(size, true);
        int correct_preds = 0;
        for (auto pred = preds.begin(); pred != preds.end(); pred++) {
            int predLoc = pred->pc % size;
            if (pred->taken == table[predLoc]) {
                correct_preds++;
            }
            else {
                table[predLoc] = !table[predLoc];
            }
        }
        ss << correct_preds << "," << preds.size() << ";";
        if (i == 7) {
            ss << "\n";
        }
        else {
            ss << " ";
        }
    }
    return ss.str();
}

std::string bimodal_predicitor_2(const std::vector<Prediction>& preds) {
    int table_sizes[] = {16, 32, 128, 256, 512, 1024, 2048};
    std::ostringstream ss;
    for (int i = 0; i < 7; i++) {
        // Table for 2-bit predictor 
        // 0 -> NN
        // 1 -> N
        // 2 -> T
        // 3 -> TT
        int size = table_sizes[i];
        std::vector<unsigned char> table(size, 3);
        int correct_preds = 0;
        for (auto pred = preds.begin(); pred != preds.end(); pred++) {
            int predLoc = pred->pc % size;
            if (pred->taken == table[predLoc] > 1) {
                correct_preds++;
                if (table[predLoc] == 2) {
                    table[predLoc] = 3;
                }
                else if (table[predLoc] == 1) {
                    table[predLoc] = 0;
                }
            }
            else if (table[predLoc] == 0 || table[predLoc] == 2) {
                table[predLoc] = 1;
            }
            else if (table[predLoc] == 1 || table[predLoc] == 3) {
                table[predLoc] = 2;
            }
            else {
                std::cout << "invalid state for bimodal predictor: " << table[predLoc]
                    << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        ss << correct_preds << "," << preds.size() << ";";
        if (i == 7) {
            ss << "\n";
        }
        else {
            ss << " ";
        }
    }
    return ss.str();
}


std::string gshare_predicitor(const std::vector<Prediction>& preds) {
    std::ostringstream ss;
    // 0000 0000 0000 0111 -> 00 07 
    // 0000 0111 1111 1111 -> 07 FF 
    for (unsigned short r_mask = 0x7; r_mask <= 0x7FF; r_mask = (r_mask << 1) | 1) {
        std::vector<unsigned char> table(2048, 3);
        unsigned short greg = 0; // Greg will keep track of things for me
        int correct_preds = 0;
        for (auto pred = preds.begin(); pred != preds.end(); pred++) {
            int predLoc = (pred->pc ^ greg) % 2048;
            if (pred->taken == table[predLoc] > 1) {
                correct_preds++;
                if (table[predLoc] == 2) {
                    table[predLoc] = 3;
                }
                else if (table[predLoc] == 1) {
                    table[predLoc] = 0;
                }
            }
            else if (table[predLoc] == 0 || table[predLoc] == 2) {
                table[predLoc] = 1;
            }
            else if (table[predLoc] == 1 || table[predLoc] == 3) {
                table[predLoc] = 2;
            }
            else {
                std::cout << "invalid state for bimodal predictor: " << table[predLoc]
                    << std::endl;
                exit(EXIT_FAILURE);
            }
            greg = (greg << 1 | pred->taken) & r_mask; 
        }
        ss << correct_preds << "," << preds.size() << ";";
        if (r_mask == 0x7FF) {
            ss << "\n";
        }
        else {
            ss << " ";
        }
    }
    return ss.str();
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
    output << bimodal_predicitor_1(preds) << "\n";
    output << bimodal_predicitor_2(preds) << "\n";
    output << gshare_predicitor(preds) << "\n";

    output.close();
}
