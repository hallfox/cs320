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
            int pred_loc = pred->pc % size;
            if (pred->taken == table[pred_loc]) {
                correct_preds++;
            }
            else {
                table[pred_loc] = !table[pred_loc];
            }
        }
        ss << correct_preds << "," << preds.size() << ";";
        if (i != 6) {
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
            int pred_loc = pred->pc % size;
            if (pred->taken == table[pred_loc] > 1) {
                correct_preds++;
                if (table[pred_loc] == 2) {
                    table[pred_loc] = 3;
                }
                else if (table[pred_loc] == 1) {
                    table[pred_loc] = 0;
                }
            }
            else if (table[pred_loc] == 0 || table[pred_loc] == 2) {
                table[pred_loc] = 1;
            }
            else if (table[pred_loc] == 1 || table[pred_loc] == 3) {
                table[pred_loc] = 2;
            }
            else {
                std::cout << "invalid state for bimodal predictor: " << table[pred_loc]
                    << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        ss << correct_preds << "," << preds.size() << ";";
        if (i != 6) {
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
            int pred_loc = (pred->pc ^ greg) % 2048;
            if (pred->taken == table[pred_loc] > 1) {
                correct_preds++;
                if (table[pred_loc] == 2) {
                    table[pred_loc] = 3;
                }
                else if (table[pred_loc] == 1) {
                    table[pred_loc] = 0;
                }
            }
            else if (table[pred_loc] == 0 || table[pred_loc] == 2) {
                table[pred_loc] = 1;
            }
            else if (table[pred_loc] == 1 || table[pred_loc] == 3) {
                table[pred_loc] = 2;
            }
            else {
                std::cout << "invalid state for bimodal predictor: " << table[pred_loc]
                    << std::endl;
                exit(EXIT_FAILURE);
            }
            greg = (greg << 1 | pred->taken) & r_mask; 
        }
        ss << correct_preds << "," << preds.size() << ";";
        if (r_mask != 0x7FF) {
            ss << " ";
        }
    }
    return ss.str();
}

std::string tournament_predicitor(const std::vector<Prediction>& preds) {
    std::ostringstream ss;
    unsigned short greg = 0; // Greg will keep track of things for me
    std::vector<unsigned char> gshare_table(2048, 3);
    std::vector<unsigned char> bimodal_table(2048, 3);
    std::vector<unsigned char> selector_table(2048, 0);
    int correct_preds = 0;
    for (auto pred = preds.begin(); pred != preds.end(); pred++) {
        int gshare_pred_loc = (pred->pc ^ greg) % 2048;
        int bimodal_pred_loc = pred->pc % 2048;

        // Select a predictor
        unsigned char &gshare_guess = gshare_table[gshare_pred_loc],
                      &bimodal_guess = bimodal_table[bimodal_pred_loc],
                      &predictor = selector_table[bimodal_pred_loc];
        if (predictor < 2) {
            // Use gshare
            if (pred->taken == gshare_guess > 1) {
                correct_preds++;
                // Update selector table if bimodal was wrong
                predictor = pred->taken == bimodal_guess > 1 ? predictor : 0;
            }
            else if (pred->taken == bimodal_guess > 1) {
                predictor++;
            }
        }
        else {
            // Use bimodal
            if (pred->taken == bimodal_guess > 1) {
                correct_preds++;
                // Update selector table if gshare was wrong
                predictor = pred->taken == gshare_guess > 1 ? predictor : 3;
            }
            else if (pred->taken == gshare_guess > 1) {
                predictor--;
            }
        }

        // Update predictors
        // Gshare
        if (pred->taken == gshare_guess > 1) {
            gshare_guess = gshare_guess > 1 ? 3 : 0;
        }
        else {
            gshare_guess = gshare_guess == 0 || gshare_guess == 2 ? 1 : 2;
        }
        // Bimodal
        if (pred->taken == bimodal_guess > 1) {
            bimodal_guess = bimodal_guess > 1 ? 3 : 0;
        }
        else {
            bimodal_guess = bimodal_guess == 0 || bimodal_guess == 2 ? 1 : 2;
        }
        
        greg = (greg << 1 | pred->taken) & 0x7FF; 
    }
    ss << correct_preds << "," << preds.size() << ";";
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
    output << tournament_predicitor(preds) << "\n";

    output.close();

    return 0;
}
