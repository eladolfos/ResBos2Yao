#ifndef BEAM_HH
#define BEAM_HH

#include <iosfwd>
#include <map>

#include "ResBos/Enums.hh"
#include "ResBos/FourVector.hh"

namespace IO {
    class Settings;
}

namespace Utility {
    class Grid2D;
    class Grid3D;
    class PDF;
    class Hoppet;
}

namespace Beam {

class Convolution;

// Beam related Enums
enum class BeamType {p,pb,n,Cu,Pb,d};
enum class BDF {
    // Default PDF
    PDF,
    // Convolutions with C function and PDF
    C,C1,C2,G1,
    // Convolutions with C function and splitting functions
    C1P1,C1P1P1,C1P2,C2P1,G1P1,
    // Convolutions with the splitting functions
    P1,P2,P3,P1P1,P1P1P1,P1P2,P2P1
};

typedef std::map<int,Utility::Grid2D*> ConvGrid;
typedef std::map<BDF,double> BDFMap;

const std::map<BDF,Splitting> BDF2Splitting = {
    {BDF::P1,     Splitting::P1},
    {BDF::P2,     Splitting::P2},
    {BDF::P3,     Splitting::P3},
    {BDF::P1P1,   Splitting::P1P1},
    {BDF::P1P2,   Splitting::P1P2},
    {BDF::P2P1,   Splitting::P2P1},
    {BDF::P1P1P1, Splitting::P1P1P1}
};

const std::map<BDF,Conv> BDF2Conv = {
    {BDF::C,      Conv::C},    
    {BDF::C1,     Conv::C1},    
    {BDF::C2,     Conv::C2},    
    {BDF::C1P1,   Conv::C1P1},    
    {BDF::C1P1P1, Conv::C1P1P1},    
    {BDF::C1P2,   Conv::C1P2},    
    {BDF::C2P1,   Conv::C2P1},    
    {BDF::G1,     Conv::G1},    
    {BDF::G1P1,   Conv::G1P1},    
};

const std::map<Splitting,BDF> Splitting2BDF = {
    {Splitting::P1,     BDF::P1},     
    {Splitting::P2,     BDF::P2},     
    {Splitting::P3,     BDF::P3},     
    {Splitting::P1P1,   BDF::P1P1},   
    {Splitting::P1P2,   BDF::P1P2},   
    {Splitting::P2P1,   BDF::P2P1},   
    {Splitting::P1P1P1, BDF::P1P1P1} 
};

const std::map<Conv,BDF> Conv2BDF = {
    {Conv::C,      BDF::C},      
    {Conv::C1,     BDF::C1},      
    {Conv::C2,     BDF::C2},      
    {Conv::C1P1,   BDF::C1P1},      
    {Conv::C1P1P1, BDF::C1P1P1},    
    {Conv::C1P2,   BDF::C1P2},      
    {Conv::C2P1,   BDF::C2P1},      
    {Conv::G1,     BDF::G1},      
    {Conv::G1P1,   BDF::G1P1}      
};

class Beam {
    public:
        // Constructors / Destructor
        Beam();
        Beam(int, IO::Settings);
        Beam(BeamType, FourVector);
        virtual ~Beam();
        static std::pair<Beam*,Beam*> MakeBeams(const std::string, const double);

        // Get information about the beam
        virtual BeamType GetBeamType() const {return beamType;}
        virtual double Energy() const {return energy;}
        inline Utility::PDF* GetPDF() const {return pdf;}
        inline Utility::Hoppet* GetHoppet() const {return hoppet;}
        inline Convolution* GetConvolution() {return convolution;}
        virtual BDFMap BeamMap(const size_t&, const int&, const double&, const double&) const;
        virtual double GetQMin() const;

        // Set information about the beam
        virtual void SetBeamEnergy(const double& energy_) {energy = energy_;}
        virtual void SetBeamType(const BeamType& beam_) {beamType = beam_;}
        virtual void SetBeamMomentum(const FourVector& mom) {beamMom = mom;}
        virtual inline void SetPDF(Utility::PDF* pdf_) {pdf = pdf_;}
        virtual inline void SetHoppet(Utility::Hoppet* hoppet_) {hoppet = hoppet_;}
        virtual inline void SetConvolution(Convolution *conv) {convolution = conv;}

        // Functions
        virtual double PDF(const BDF, const int, const double, const double) const;
        
        // Stream Operators
        friend std::ostream& operator<<(std::ostream&, const Beam&);

    private:
        // Functions
        double BeamPDF(const int, const double, const double) const;
        double BeamHoppet(const int, const double, const double, const Splitting) const;
        double BeamConv(const int, const double, const double, const Conv) const;
        int Neutron(const int) const;

        // Variables
        double energy;
        BeamType beamType;
        Utility::PDF *pdf;
        Utility::Hoppet *hoppet;
        Convolution *convolution;
        FourVector beamMom;
};

}

#endif
