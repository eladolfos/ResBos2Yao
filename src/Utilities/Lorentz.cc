#include "ResBos/Lorentz.hh"
#include "ResBos/FourVector.hh"
#include "ResBos/ThreeVector.hh"

Lorentz::Lorentz(const FourVector &v1, const FourVector &v2, const size_t mode) 
    : m_status(mode ? 3 : 2), m_beta{1, 0, 0, 0}, m_rsq{1.}, m_usen{false} {

    if(m_status == 3) {
        m_beta = v1;
        m_n = v2;
        return;
    }

    ThreeVector b(v1.GetVect()), a(v2.GetVect());
    m_ct = a*b/(a.Abs()*b.Abs());
    m_n = FourVector(0.0, a.Cross(b));
    m_usen = m_n.P() != 0;
    if(!m_usen) {
        m_n = FourVector(0.0, a.Cross(FourVector::XVEC.GetVect()));
        m_usen = m_n.P() != 0;
        if(!m_usen) {
            m_n = FourVector(0.0, a.Cross(FourVector::YVEC.GetVect()));
            m_usen = m_n.P() != 0;
            if(!m_usen) {
                m_n = FourVector(0.0, a.Cross(FourVector::ZVEC.GetVect()));
                m_usen = m_n.P() != 0;
            }
        }
    }

    m_usen &= m_ct < 1.0;
    m_nsq = m_n.P2();
    m_nabs = sqrt(m_nsq);
    if(m_ct*m_ct > 1.0) {
        m_ct = m_ct > 0.0 ? 1.0 : -1.0;
    }
    m_st = -sqrt(1.0-m_ct*m_ct);
    if(m_usen) {
        a = a.Unit();
        b = b.Unit();
        ThreeVector n(m_n.GetVect()), at(n*(n*a)/m_nsq), ap(a-at);
        ThreeVector ta(at+m_ct*ap-m_st/m_nabs*n.Cross(ap));
    }
}

Lorentz::Lorentz(const FourVector &v) : m_status(1), m_beta(v), m_rsq(v.M()), m_usen(false) {
    if(m_rsq == 0 || m_rsq == -m_beta[0]) m_status = 0;
}

void Lorentz::Boost(FourVector &v) const {
    if(m_status == 0) return;
    double v0((m_beta[0]*v[0]-m_beta.GetVect()*v.GetVect())/m_rsq);
    double c1((v[0]+v0)/(m_rsq+m_beta[0]));
    v = FourVector(v0, v.GetVect()-c1*m_beta.GetVect());
}

void Lorentz::BoostBack(FourVector &v) const {
    if(m_status == 0) return;
    double v0((m_beta[0]*v[0]+m_beta.GetVect()*v.GetVect())/m_rsq);
    double c1((v[0]+v0)/(m_rsq+m_beta[0]));
    v = FourVector(v0, v.GetVect()+c1*m_beta.GetVect());
}

void Lorentz::Rotate(FourVector &v) const {
    if(m_usen) {
        ThreeVector n(m_n.GetVect()), a(v.GetVect()), at(n*(n*a)/m_nsq), ap(a-at);
        v = FourVector(v[0], at+m_ct*ap+m_st/m_nabs*n.Cross(ap));
    }
}

void Lorentz::RotateBack(FourVector &v) const {
    if(m_usen) {
        ThreeVector n(-m_n.GetVect()), a(v.GetVect()), at(n*(n*a)/m_nsq), ap(a-at);
        v = FourVector(v[0], at+m_ct*ap+m_st/m_nabs*n.Cross(ap));
    }
}

void Lorentz::Invert() {
    if(m_status == 3) {
        std::swap<FourVector>(m_beta, m_n);
        return;
    }
    for(size_t i = 1; i < 4; ++i) {
        m_beta[i] = -m_beta[i];
        m_n[i] = -m_n[i];
    }
}
