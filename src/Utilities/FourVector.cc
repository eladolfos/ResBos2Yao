#include <stdexcept>
#include <cmath>
#include <iostream>

#include "ResBos/FourVector.hh"
#include "ResBos/ThreeVector.hh"

const FourVector FourVector::XVEC(1, 0, 0, 1);
const FourVector FourVector::YVEC(0, 1, 0, 1);
const FourVector FourVector::ZVEC(0, 0, 1, 1);

FourVector::FourVector(const double& px, const double& py, const double& pz, const double& e) {
    p[0] = px;
    p[1] = py;
    p[2] = pz;
    p[3] = e;
}

FourVector::FourVector(const std::vector<double>& p_) {
    if(p_.size() != 4) throw std::runtime_error("Four Vector given " + std::to_string(p_.size()) + " parameters, but expects 4 parameters");
    p = {p_[0],p_[1],p_[2],p_[3]};
}

FourVector::FourVector(const FourVector& other) {
    p = other.p;
}

FourVector::FourVector(const double &E, const ThreeVector &v) {
    p = {v[0], v[1], v[2], E};
}

FourVector::~FourVector() {}

double FourVector::M() const {
    double m2 = M2();
    if(std::abs(m2) < 1E-2) return 0;
    return sqrt(m2);
}

double FourVector::Pt2() const {
    return p[0]*p[0]+p[1]*p[1];
}

double FourVector::Pt() const {
    return sqrt(Pt2());
}

double FourVector::Phi() const {
    double phi = atan2(p[1],p[0]);
    if(phi < 0) return phi + 2*M_PI;
    return phi;
}

double FourVector::CosTheta() const {
    return std::max(std::min(p[2]/P(), 1.0), -1.0);
}

double FourVector::Theta() const {
    return std::acos(CosTheta());
}

double FourVector::Rapidity() const {
    return 0.5*log((p[3]+p[2])/(p[3]-p[2]));
}

double FourVector::DeltaR(const FourVector& v) const {
    double DEta = Rapidity() - v.Rapidity();
    double DPhi = Phi() - v.Phi();
    return sqrt(DEta*DEta+DPhi*DPhi);
}

double FourVector::Angle(const FourVector &other) const {
    const ThreeVector self3 = GetVect();
    const ThreeVector other3 = other.GetVect();
    return self3.Angle(other3);
}

double FourVector::Angle(const ThreeVector &other) const {
    const ThreeVector self3 = GetVect();
    return self3.Angle(other);
}

ThreeVector FourVector::GetVect() const {
    return ThreeVector(p[0],p[1],p[2]);
}

void FourVector::SetPxPyPzE(const double& px, const double& py, const double& pz, const double& e) {
    p = {px,py,pz,e};
}

void FourVector::SetPxPyPzE(const std::vector<double>& p_) {
    if(p_.size() != 4) throw std::runtime_error("Four Vector only given " + std::to_string(p_.size()) + " parameters, but expects 4 parameters");
    p = {p_[0],p_[1],p_[2],p_[3]};
}

void FourVector::SetVectM(const ThreeVector& p_, const double& mass) {
    p[0] = p_.Px();
    p[1] = p_.Py();
    p[2] = p_.Pz();
    p[3] = sqrt(mass*mass + p_*p_);
}

void FourVector::Boost(const ThreeVector& boost_vector) {
    Boost(boost_vector[0],boost_vector[1],boost_vector[2]);
}

void FourVector::Boost(const double& beta_x, const double& beta_y, const double& beta_z) {
    double beta2 = beta_x*beta_x+beta_y*beta_y+beta_z*beta_z;
    double gamma = 1.0/sqrt(1.0-beta2);
    double betap = beta_x*p[0]+beta_y*p[1]+beta_z*p[2];
    double gamma2 = beta2 > 0 ? (gamma-1.0)/beta2 : 0.0;

    p[0] += gamma2*betap*beta_x+gamma*beta_x*p[3];
    p[1] += gamma2*betap*beta_y+gamma*beta_y*p[3];
    p[2] += gamma2*betap*beta_z+gamma*beta_z*p[3];
    p[3] = gamma*(p[3]+betap);
}

ThreeVector FourVector::BoostVector() const {
    return ThreeVector(p[0]/p[3],p[1]/p[3],p[2]/p[3]);
}

double FourVector::Dot(const FourVector& other) const {
    return *this * other;
}

FourVector& FourVector::operator=(const FourVector& other) {
    p = other.p;

    return *this;
}

FourVector& FourVector::operator+=(const FourVector& other) {
    p[0] += other.p[0];
    p[1] += other.p[1];
    p[2] += other.p[2];
    p[3] += other.p[3];

    return *this;
}

FourVector& FourVector::operator-=(const FourVector& other) {
    p[0] -= other.p[0];
    p[1] -= other.p[1];
    p[2] -= other.p[2];
    p[3] -= other.p[3];

    return *this;
}

FourVector& FourVector::operator*=(const double& scale) {
    p[0] *= scale;
    p[1] *= scale;
    p[2] *= scale;
    p[3] *= scale;

    return *this;
}

FourVector& FourVector::operator/=(const double& scale) {
    p[0] /= scale;
    p[1] /= scale;
    p[2] /= scale;
    p[3] /= scale;

    return *this;
}

double FourVector::operator*(const FourVector& other) const {
    return p[3]*other.p[3] - (p[0]*other.p[0] + p[1]*other.p[1] + p[2]*other.p[2]);
}

FourVector FourVector::operator-() const {
   return FourVector(-p[0],-p[1],-p[2],-p[3]);
}

FourVector FourVector::operator+() const {
    return FourVector(*this);
}

FourVector FourVector::operator*(const double& scale) const {
    return FourVector(*this)*=scale;
}

FourVector FourVector::operator/(const double& scale) const {
    return FourVector(*this)/=scale;
}

FourVector FourVector::operator+(const FourVector& other) const {
    return FourVector(*this)+=other;
}

FourVector FourVector::operator-(const FourVector& other) const {
    return FourVector(*this)-=other;
}

bool FourVector::operator==(const FourVector& other) const {
    return p==other.p;
}

double FourVector::operator()(const size_t& i) const {
    return p[i];
}

double FourVector::operator[](const size_t& i) const {
    return p[i];
}

double& FourVector::operator()(const size_t& i) {
    return p[i];
}

double& FourVector::operator[](const size_t& i) {
    return p[i];
}

std::ostream& operator<<(std::ostream& os, const FourVector& vec) {
    os << "FourVector( " << vec.Px() << ", " << vec.Py() << ", " << vec.Pz() << ", " << vec.E() << ")";
    return os;
}

std::istream& operator>>(std::istream& is, FourVector& vec) {
    std::string head(11, ' '), sep1(1, ' '), sep2(1, ' '),
        sep3(1, ' '), tail(1, ' ');
    double px, py, pz, e;
    is.read(&head[0], 11);
    is >> px;
    is.read(&sep1[0], 1);
    is >> py;
    is.read(&sep2[0], 1);
    is >> pz;
    is.read(&sep3[0], 1);
    is >> e;
    is.read(&tail[0], 1);
    if(head == "FourVector(" &&
       sep1 == "," && sep2 == "," && 
       sep3 == "," && tail == ")") 
        vec = FourVector(px,py,pz,e);
    return is;
}

FourVector operator*(const double& s, const FourVector& v) {
    return v*s;
}
