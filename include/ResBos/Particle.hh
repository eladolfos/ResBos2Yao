#ifndef PARTICLE_HH
#define PARTICLE_HH

#include "ResBos/FourVector.hh"

class Particle {
    public:
        // Constructors
        Particle() = default;
        Particle(const int& pid_, const FourVector& mom_, const int& status_, const std::pair<int,int>& cols_ = std::make_pair(0,0)) 
            : pid{pid_}, status{status_}, mom{mom_}, cols{cols_} {
                mother1 = 0; mother2 = 0; daughter1 = 0; daughter2 = 0; 
            }

        // Destructor
        virtual ~Particle() = default;

        // Getters
        virtual int PID() const {return pid;}
        virtual int Status() const {return status;}
        virtual FourVector Mom() const {return mom;}
        virtual std::pair<int,int> Cols() const {return cols;}
        virtual int Col() const {return cols.first;}
        virtual int Acol() const {return cols.second;}
        virtual bool isFinal() const {
            if(status == 1) return true;
            return false;
        }
        virtual double Px() const {return mom.Px();}
        virtual double Py() const {return mom.Py();}
        virtual double Pz() const {return mom.Pz();}
        virtual double E() const {return mom.E();}
        virtual double M() const {return mom.M();}
        virtual size_t Mother1() const {return mother1;}
        virtual size_t Mother2() const {return mother2;}
        virtual size_t Daughter1() const {return daughter1;} 
        virtual size_t Daughter2() const {return daughter2;}

        // Setters
        virtual void SetPID(const int& pid_) {pid = pid_;}
        virtual void SetStatus(const int& status_) {status = status_;}
        virtual void SetMom(const FourVector& mom_) {mom = mom_;}
        virtual void SetCols(const std::pair<int,int>& cols_) {cols = cols_;}
        virtual void SetMothers(const size_t& m1, const size_t& m2) {mother1 = m1; mother2 = m2;}
        virtual void SetDaughters(const size_t& d1, const size_t& d2) {daughter1 = d1; daughter2 = d2;}

    private:
        int pid, status;
        FourVector mom;
        std::pair<int,int> cols;
        size_t mother1, mother2;
        size_t daughter1, daughter2;
};

#endif
