#ifndef PYRESBOS_HH
#define PYRESBOS_HH

#include <memory>
#include <string>
#include <vector>

namespace ResBos {

class ResBos;

class pyResbos {
    public:
        pyResbos(int argc, char* argv[]);
        double MatrixElement(std::vector<double>);
        std::vector<double> MatrixElementVec(int nevents, int vars, double* array);

        double GetCalculation(std::vector<double>) {};
        std::vector<double> GetCalculationVec(int nevents, int vars, double* array) {};

        std::vector<double> GetResummation(int nevents, int vars, double* array, int mode);

    private:
        std::shared_ptr<ResBos> resbos;
};

}

#endif
