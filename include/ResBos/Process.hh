#ifndef PROCESS_HH
#define PROCESS_HH

#include <vector>
#include <unordered_map>
#include <map>
#include <memory>

#include "ResBos/Electroweak.hh"
#include "ResBos/Enums.hh"
#include "ResBos/Event.hh"
#include "ResBos/FourVector.hh"
#include "ResBos/PhaseSpace.hh"

class Electroweak;
class PhaseSpace;

namespace IO {
    class Settings;
}

namespace ResBos {
    class ResBos;

    typedef std::vector<std::pair<int,int>> Partons;

    class Process {
        public:
            // General setup for the process class
            Process() {
                ps = nullptr;
                ew = nullptr;
            }
            Process(std::shared_ptr<ResBos>, const IO::Settings&);
            virtual ~Process();
            
            virtual void Initialize(const IO::Settings&) = 0;
            virtual void InitializeMCFM() = 0;
            virtual bool IsRegistered() = 0;

            // Information about the calculation
//            virtual inline const Electroweak GetEW() const {return *ew;}
            virtual inline Electroweak* GetEW() const {return ew.get();}
            virtual inline PhaseSpace* GetPS() const {return ps.get();}
            virtual Partons GetPartons(const size_t &i = 0) const = 0;
            virtual size_t GetChannels() const = 0;
            virtual size_t GetNAngular() const = 0;
            virtual size_t GetResumAngular() const = 0;
            virtual InitialState GetInitState() const = 0;
            virtual const std::vector<int>& PIDs() const = 0;
            std::string Name() const {return name;}
            virtual double Charge(const int&, const size_t&) const { return 1; }
            virtual int GetAlphasCoupling() const = 0;

            // Information pertaining to the hard part and matrix element
            virtual size_t GetNDim() const = 0;
            virtual double H1() const = 0;
            virtual double H2(const double &Q) const = 0;
            virtual double H2(const size_t &nf = 5) const = 0;
            virtual double ME(const std::vector<double>&, const std::vector<double>&) const = 0;
            virtual double GetCoupling(const double&) const = 0;
            virtual double CxFCxF(const double&, const double&,
                                  const double&, const size_t&) const = 0;
            virtual double GetPhaseSpacePoint(const std::vector<double>&, std::vector<double>&,
                                              std::vector<FourVector>&) const = 0;
            virtual double GetPert(const double&, const double&, const double&,
                                   const double&, const double&, const double&,
                                   const double&, const double&, const size_t&) const = 0;
            virtual double NonPert(const std::array<double, 2>&, const std::vector<double>&, bool=false) const;
            virtual std::array<double, 4> CalcCorrection(double, const std::vector<double>&, bool) const;

            // Function to generate an event to be passed to LHE
            virtual void MakeEvent(Event&, const std::vector<FourVector>&) = 0;

        protected:
            std::shared_ptr<ResBos> resbos;
            std::unique_ptr<PhaseSpace> ps;
            // Partons partons;
            std::unique_ptr<Electroweak> ew;
            std::string name;
    };

    class ProcessFactory {
        public: 
            using TCreateMethod = std::unique_ptr<Process>(*)(std::shared_ptr<ResBos>, const IO::Settings&);
            TCreateMethod CreateFunc;

            static ProcessFactory& Instance();

            bool Register(const std::string&, TCreateMethod);
            std::unique_ptr<Process> Create(const std::string&, std::shared_ptr<ResBos>, const IO::Settings&);

        private:
            ProcessFactory() : methods() {};
            std::map<std::string, TCreateMethod> methods;
    };

#define REGISTER(process) \
    bool process::registered = ProcessFactory::Instance().Register(process::GetName(), \
            process::Create);

}

#endif
