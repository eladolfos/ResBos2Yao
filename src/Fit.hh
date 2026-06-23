#ifndef FIT_HH
#define FIT_HH

#include <vector>
#include <algorithm>
#include <cmath>
#include <map>
#include <iostream>
#include <iterator>
#include <fstream>
#include <sstream>
#include <cstdlib>

#include "TMinuit.h"
#include "TGraph.h"

class Process;
class PDF;
class Settings;

// Class to contain all the information of an experiment needed to fit the non-perturbative function
class Experiment {
    public:
        Experiment(std::string name, Settings *s); 
        void CalcChi2();
        void CalcTheory();
        void SetG(double g1New, double g2New, double g3New);
        inline void SetNorm(double Norm) { N = Norm; };
        inline double GetChi2() { return chi2; };
        inline int GetID() { return id; };
        void SetupProcess(Settings *s);

    private:
        void CalcShift();
        int id;
        std::vector<double> Qmin, Qmax, pT, dataPt, statError, sysError, err, yk, theory, corrErr;
        std::map<double,std::vector<double>> EvalPoints;
        std::map<double,double> CxfCxf, PertSud;
        std::map<std::vector<double>,double> yPiece;
        double frac_n, ecm, yPz, xsec;
        double chi2, g1, g2, g3, N, Q0;
        double A, B, rk;
        int iBeam, AOrder, BOrder, COrder, HOrder;
        PDF *pdf;
        Process *p;
};

void FitNonPert(const int nparm, std::vector<std::string> expNames, Settings *s, std::vector<double> parms, std::vector<double> step,
        std::vector<double> min, std::vector<double> max, std::vector<std::string> parmName);

#endif
