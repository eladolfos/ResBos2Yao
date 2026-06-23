#ifndef LORENTZ_HH
#define LORENTZ_HH

#include <array>
#include <vector>

#include "ResBos/FourVector.hh"
#include "ResBos/ThreeVector.hh"

class Lorentz {
    public:
        Lorentz() : m_status{0}, m_beta{1, 0, 0, 0}, m_rsq{1.}, m_usen{false} {}
        Lorentz(const FourVector&, const FourVector&, const size_t mode=0);
        Lorentz(const FourVector&);

        void Boost(FourVector&) const;
        void BoostBack(FourVector&) const;

        void Rotate(FourVector&) const;
        void RotateBack(FourVector&) const;

        void Invert();
        FourVector operator*(const FourVector &vec) const {
            FourVector result(vec);
            if(m_status == 1) Boost(result);
            else if(m_status == 2) Rotate(result);
            return result;
        }

    private:
        size_t m_status;
        FourVector m_beta, m_n;
        double m_rsq, m_ct, m_st, m_nsq, m_nabs;
        bool m_usen;
};

#endif
