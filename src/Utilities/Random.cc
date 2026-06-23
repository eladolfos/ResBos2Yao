#include <cmath>
#include <cfloat>
#include <vector>
#include <limits>
#include <algorithm>
#include <functional>

#include "ResBos/Random.hh"

// Seed mt based on discussion at: https://codereview.stackexchange.com/questions/109260/seed-stdmt19937-from-stdrandom-device 
template<class T = std::mt19937, std::size_t N = T::state_size>
T RandomlySeededMersenneTwister() {
    typename T::result_type random_data[N];
    std::random_device source;
    std::generate(std::begin(random_data), std::end(random_data), std::ref(source));
    std::seed_seq seeds(std::begin(random_data), std::end(random_data));
    T seededEngine(seeds);
    return seededEngine;
}

Random::Random() {
    std::random_device rd;
    mt = RandomlySeededMersenneTwister();
    dist = std::uniform_real_distribution<>(0.0,std::nextafter(1.0,std::numeric_limits<double>::max()));
}

Random::Random(const size_t& seed) {
    mt.seed(seed);
    dist = std::uniform_real_distribution<>(0.0,std::nextafter(1.0,std::numeric_limits<double>::max()));
}

void Random::Seed(const size_t& seed) {
    mt.seed(seed);
    dist = std::uniform_real_distribution<>(0.0,std::nextafter(1.0,std::numeric_limits<double>::max()));
}

std::vector<double> Random::Get(const size_t& n) {
    std::vector<double> result(n);
    for(size_t i = 0; i < n; ++i) result[i]=(dist(mt));
    return result;
}

std::vector<std::vector<double>> Random::Get(const size_t& dim1, const size_t& dim2) {
    std::vector<std::vector<double>> result;
    for(size_t i = 0; i < dim1; ++i) {
        std::vector<double> tmp(dim2);
        result.push_back(Get(dim2));
    }
    return result;
}

double Random::Get() {
    return dist(mt);
}
