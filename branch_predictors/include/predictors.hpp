#include <string>
#include <vector>

#ifndef _PREDICTOR_H
#define _PREDICTOR_H

typedef struct Prediction {
    bool taken;
    unsigned long long pc;

    Prediction(std::string behavior, unsigned long long pc):
        pc(pc), taken(behavior == "T") {}
} Prediction;

std::string yes_predicitor(const std::vector<Prediction>&);
std::string no_predicitor(const std::vector<Prediction>&);
std::string bimodal_predicitor_1(const std::vector<Prediction>&);
std::string bimodal_predicitor_2(const std::vector<Prediction>&);
std::string gshare_predicitor(const std::vector<Prediction>&);
std::string tournament_predicitor(const std::vector<Prediction>&);

#endif
