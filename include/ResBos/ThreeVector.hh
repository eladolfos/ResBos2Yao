#ifndef THREEVECTOR_HH
#define THREEVECTOR_HH

#include <vector>
#include <array>
#include <iosfwd>
#include <cmath>

template<class T, size_t N>
constexpr size_t getsize(T(&)[N]) {return N;}

class ThreeVector {
    public:
        // Constructors
        ThreeVector() = default;
        ThreeVector(const double&, const double&, const double&);
        ThreeVector(const std::vector<double>&);
        ThreeVector(const std::array<double,3>& p_) : p(p_) {}
        ThreeVector(const ThreeVector&);
        constexpr ThreeVector(ThreeVector&&) = default;
        virtual ~ThreeVector() {};

        // Getters
        double Abs() const { return std::sqrt(Abs2()); }
        double Abs2() const { return p[0]*p[0] + p[1]*p[1] + p[2]*p[2]; }
        virtual double Px() const {return p[0];}
        virtual double Py() const {return p[1];}
        virtual double Pz() const {return p[2];}
        
        // Setters
        virtual void SetPxPyPz(const double&, const double&, const double&);
        virtual void SetPxPyPz(const std::vector<double>&);
        virtual void SetPxPyPz(const std::array<double,3>& p_) {p = p_;}
        virtual void SetPx(const double& px) {p[0] = px;}
        virtual void SetPy(const double& py) {p[1] = py;}
        virtual void SetPz(const double& pz) {p[2] = pz;}

        // Functions
        virtual double Dot(const ThreeVector&) const;
        ThreeVector Cross(const ThreeVector&) const;
        double Angle(const ThreeVector&) const;
        ThreeVector Unit() const { 
            double norm = std::sqrt(p[0]*p[0]+p[1]*p[1]+p[2]*p[2]);
            return ThreeVector(p[0]/norm, p[1]/norm, p[2]/norm);
        }

        // Operator Overloads
        ThreeVector& operator=(const ThreeVector&); 
        ThreeVector& operator+=(const ThreeVector&);
        ThreeVector& operator-=(const ThreeVector&);
        ThreeVector& operator*=(const double&);
        ThreeVector& operator/=(const double&);

        double operator*(const ThreeVector&) const;
        ThreeVector operator-() const;
        ThreeVector operator+() const;
        ThreeVector operator*(const double&) const;
        ThreeVector operator/(const double&) const;
        ThreeVector operator+(const ThreeVector&) const;
        ThreeVector operator-(const ThreeVector&) const;

        // Comparison Operators
        bool operator==(const ThreeVector&) const;
        bool operator!=(const ThreeVector& other) const {return !(*this == other);}

        // Access Operators
        double operator()(const size_t &i) const {
            return p[i];
        }
        double operator[](const size_t &i) const {
            return p[i];
        }
        double& operator()(const size_t &i) {
            return p[i];
        }
        double& operator[](const size_t &i) {
            return p[i];
        }

        // Stream Operators
        friend std::ostream& operator<<(std::ostream&, const ThreeVector&); 
        friend std::istream& operator>>(std::istream&, ThreeVector&); 

    private:
        std::array<double,3> p;
};

ThreeVector operator*(const double&, const ThreeVector&);

#endif
