#include <string>
#include <vector>

#ifndef _PREDICTOR_H
#define _PREDICTOR_H

typedef struct Prediction {
    bool taken;
    unsigned long long pc;

    Prediction(std::string behavior, unsigned long long pc): pc(pc) {
        if (behavior == "T") {
            this->taken = true;
        }
        else {
            this->taken = false;
        }
    }
} Prediction;

std::string yes_predicitor(const std::vector<Prediction>&);
std::string no_predicitor(const std::vector<Prediction>&);
std::string bimodal_predicitor(const std::vector<Prediction>&);

#endif
