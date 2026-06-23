#ifndef ENUMS_HH
#define ENUMS_HH

#include <stdexcept>
#include <string>

//Enumerate the calculation enums
enum class GridType {Total, Resum, Asym, DelSig, Pert, Y, WmA};
enum class Scheme {CFG, CSS}; 
enum class Conv {C, C1, C2, C1P1, C1P1P1, C1P2, C2P1, G1, G1P1}; 
enum class Splitting {P1 = 1, P2 = 2, P3 = 3, P1P1 = 11, P1P1P1 = 111, P1P2 = 12, P2P1 = 21}; 
enum class NonPertEnum {BLNY,SIYY,SIYYgy,IY,IY1,IY2,IY6,Gaussian,TMD,None};
enum class A5Choice {Central, Plus, Minus};
enum class CentralScale {Q,MT};

enum class InitialState {qq,gg,qg};

// Enumerate the different variations
enum class VarType {PDF,Scale};

// Enumerate the different EW schemes implemented
enum class EWInputs {GFMWMZ,AMWMZ,ASWMZ,ASWGF,AGFMZ,EMWMZ,ESWMZ,ESWGF};
enum class EWMassScheme {OnShell,PoleScheme,ComplexMass};

// Enumerate Histogram outputs
enum class HistOut {Native,YODA,ROOT};

// Compare grid type to letter from saved grid
inline GridType ToGridType(const char s) {
    if(s == 'T') return GridType::Total;
    else if(s == 'R') return GridType::Resum;
    else if(s == 'A') return GridType::Asym;
    else if(s == 'D') return GridType::DelSig;
    else if(s == 'P') return GridType::Pert;
    else if(s == 'W') return GridType::WmA;
    else if(s == 'Y') return GridType::Y;
    else throw std::runtime_error("Grid is corrupt. " + std::to_string(s) + " is not a valid grid type");
}

#endif
