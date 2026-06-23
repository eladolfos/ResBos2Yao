#ifndef RANDOM_HH
#define RANDOM_HH

#include <random>

class Random {
    public:
        Random();
        Random(const size_t&);
        virtual ~Random() {}

        virtual void Seed(const size_t&);
        virtual std::vector<double> Get(const size_t&);
        virtual std::vector<std::vector<double>> Get(const size_t&, const size_t&);
        virtual double Get();

    private:
        std::mt19937 mt;
        std::uniform_real_distribution<> dist;
};

#endif
