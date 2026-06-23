#ifndef FOURVECTOR_HH
#define FOURVECTOR_HH

#include <vector>
#include <array>
#include <iosfwd>
#include <cmath>

class ThreeVector;

class FourVector {
    public:
        // Constructors
        FourVector() { p = {0,0,0,0}; }
        virtual ~FourVector();
        FourVector(const double&, const double&, const double&, const double&);
        FourVector(const std::vector<double>&);
        FourVector(const std::array<double,4>& p_) : p(p_) {}
        FourVector(const FourVector&);
        FourVector(const double&, const ThreeVector&);
        constexpr FourVector(FourVector&&) = default;

        // Getters
        double P2() const { return Pt2()+p[2]*p[2]; }
        double P() const { return std::sqrt(P2()); }
        virtual double Px() const {return p[0];}
        virtual double Py() const {return p[1];}
        virtual double Pz() const {return p[2];}
        virtual double E() const {return p[3];}
        virtual double M2() const {return (*this)*(*this);}
        virtual double M() const;
        virtual double Pt2() const;
        virtual double Pt() const;
        virtual double Phi() const;
        virtual double Rapidity() const;
        double CosTheta() const;
        double Theta() const;
        virtual double DeltaR(const FourVector&) const;
        virtual ThreeVector GetVect() const;
        FourVector Perp() const { return FourVector(p[0], p[1], 0, 0); }
        
        // Setters
        virtual void SetPxPyPzE(const double&, const double&, const double&, const double&);
        virtual void SetPxPyPzE(const std::vector<double>&);
        virtual void SetPxPyPzE(const std::array<double,4>& p_) {p = p_;}
        virtual void SetPx(const double& px) {p[0] = px;}
        virtual void SetPy(const double& py) {p[1] = py;}
        virtual void SetPz(const double& pz) {p[2] = pz;}
        virtual void SetE(const double& e) {p[3] = e;}
        virtual void SetVectM(const ThreeVector&, const double&);

        // Functions
        virtual void Boost(const ThreeVector&);
        virtual void Boost(const double&, const double&, const double&);
        virtual ThreeVector BoostVector() const;
        virtual double Dot(const FourVector&) const;
        virtual double Angle(const FourVector&) const;
        virtual double Angle(const ThreeVector&) const;

        // Operator Overloads
        FourVector& operator=(const FourVector&); 
        FourVector& operator+=(const FourVector&);
        FourVector& operator-=(const FourVector&);
        FourVector& operator*=(const double&);
        FourVector& operator/=(const double&);

        double operator*(const FourVector&) const;
        FourVector operator-() const;
        FourVector operator+() const;
        FourVector operator*(const double&) const;
        FourVector operator/(const double&) const;
        FourVector operator+(const FourVector&) const;
        FourVector operator-(const FourVector&) const;

        // Comparison Operators
        bool operator==(const FourVector &other) const;
        inline bool operator!=(const FourVector& other) const {return !(*this == other);}

        // Access Operators
        double operator()(const size_t&) const; 
        double operator[](const size_t&) const;
        double& operator()(const size_t&); 
        double& operator[](const size_t&);

        // Stream Operators
        friend std::ostream& operator<<(std::ostream&, const FourVector&); 
        friend std::istream& operator>>(std::istream&, FourVector&); 

        // Static axes
        static const FourVector XVEC;
        static const FourVector YVEC;
        static const FourVector ZVEC;

    private:
        std::array<double,4> p;
};

FourVector operator*(const double& s, const FourVector& v);

#endif
