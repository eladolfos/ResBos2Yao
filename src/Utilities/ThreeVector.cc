#include <stdexcept>
#include <cmath>
#include <iostream>

#include "ResBos/ThreeVector.hh"

ThreeVector::ThreeVector(const double& px, const double& py, const double& pz) {
    p[0] = px;
    p[1] = py;
    p[2] = pz;
}

ThreeVector::ThreeVector(const std::vector<double>& p_) {
    if(p_.size() != 3) throw std::runtime_error("Three Vector only given " + std::to_string(p_.size()) + " parameters, but expects 3 parameters");
    p = {p_[0],p_[1],p_[2]};
}

ThreeVector::ThreeVector(const ThreeVector& other) {
    p = other.p;
}

void ThreeVector::SetPxPyPz(const double& px, const double& py, const double& pz) {
    p = {px,py,pz};
}

void ThreeVector::SetPxPyPz(const std::vector<double>& p_) {
    if(p_.size() != 3) throw std::runtime_error("Three Vector only given " + std::to_string(p_.size()) + " parameters, but expects 3 parameters");
    p = {p_[0],p_[1],p_[2]};
}

double ThreeVector::Dot(const ThreeVector& other) const {
    return *this * other;
}

ThreeVector ThreeVector::Cross(const ThreeVector &other) const {
    return {p[1]*other[2]-p[2]*other[1],
            p[2]*other[0]-p[0]*other[2],
            p[0]*other[1]-p[1]*other[0]};
}

double ThreeVector::Angle(const ThreeVector &other) const {
    const double dotProd = Unit().Dot(other.Unit());
    if(dotProd > 1.0) return 0.0;
    if(dotProd < -1.0) return M_PI;
    return std::acos(dotProd);
}

ThreeVector& ThreeVector::operator=(const ThreeVector& other) {
    p = other.p;

    return *this;
}

ThreeVector& ThreeVector::operator+=(const ThreeVector& other) {
    p[0] += other.p[0];
    p[1] += other.p[1];
    p[2] += other.p[2];

    return *this;
}

ThreeVector& ThreeVector::operator-=(const ThreeVector& other) {
    p[0] -= other.p[0];
    p[1] -= other.p[1];
    p[2] -= other.p[2];

    return *this;
}

ThreeVector& ThreeVector::operator*=(const double& scale) {
    p[0] *= scale;
    p[1] *= scale;
    p[2] *= scale;

    return *this;
}

ThreeVector& ThreeVector::operator/=(const double& scale) {
    p[0] /= scale;
    p[1] /= scale;
    p[2] /= scale;

    return *this;
}

double ThreeVector::operator*(const ThreeVector& other) const {
    return p[0]*other.p[0] + p[1]*other.p[1] + p[2]*other.p[2];
}

ThreeVector ThreeVector::operator-() const {
   return ThreeVector(-p[0],-p[1],-p[2]);
}

ThreeVector ThreeVector::operator+() const {
    return ThreeVector(*this);
}

ThreeVector ThreeVector::operator*(const double& scale) const {
    return ThreeVector(*this)*=scale;
}

ThreeVector ThreeVector::operator/(const double& scale) const {
    return ThreeVector(*this)/=scale;
}

ThreeVector ThreeVector::operator+(const ThreeVector& other) const {
    return ThreeVector(*this)+=other;
}

ThreeVector ThreeVector::operator-(const ThreeVector& other) const {
    return ThreeVector(*this)-=other;
}

ThreeVector operator*(const double& s, const ThreeVector& vec) {
    return vec*s;
}

bool ThreeVector::operator==(const ThreeVector& other) const {
    return p==other.p;
}

std::ostream& operator<<(std::ostream& os, const ThreeVector& vec) {
    os << "ThreeVector( " << vec.Px() << ", " << vec.Py() << ", " << vec.Pz() << ")";
    return os;
}

std::istream& operator>>(std::istream& is, ThreeVector& vec) {
    std::string head(12,' '), sep1(1,' '), sep2(1,' '), tail(1,' ');
    double px, py, pz;
    is.read(&head[0],12);
    is >> px;
    is.read(&sep1[0],1);
    is >> py;
    is.read(&sep2[0],1);
    is >> pz;
    is.read(&tail[0],1);
    if(head == "ThreeVector(" && sep1 == "," && sep2 == "," && tail == ")")
        vec = ThreeVector(px,py,pz);
    return is;
}
