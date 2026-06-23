#ifndef RESBOS2_HH
#define RESBOS2_HH

#include "ResBos/Beam.hh"
#include "ResBos/Calculation.hh"
#include "ResBos/Convolution.hh"
#include "ResBos/Enums.hh"
#include "ResBos/Grid3D.hh"
#include "ResBos/HoppetInterface.hh"
#include "ResBos/Process.hh"
#include "ResBos/Pdf.hh"
#include "ResBos/ResBos.hh"
#include "ResBos/Settings.hh"
#include "ResBos/ThreadPool.hh"
#include "ResBos/Utility.hh"
#include "ResBos/Vegas.hh"

#include <algorithm>

// Grid Generation Functions
//std::map<std::string,Utility::Grid3D*> GridGen(Process::Process* p, IO::Settings* s, Grids g);
//std::vector<double> GetVect(std::string);
//void WriteHeader(IO::Settings* s, std::ofstream &output, Grids g);

// Variations (Scale and PDF)
//std::vector<Calc::Variation*> GetVariations(IO::Settings *s, Process::Process *p, Utility::Hoppet *h, Calc::Convolution *c);

//#include <cxxopts.hpp>

#endif
