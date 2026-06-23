#ifndef EXPERIMENT_HH
#define EXPERIMENT_HH

#include "ResBos/Beam.hh"
#include "ResBos/Calculation.hh"
#include "ResBos/Convolution.hh"
#include "ResBos/Enums.hh"
#include "ResBos/Grid3D.hh"
#include "ResBos/HoppetInterface.hh"
#include "ResBos/Process.hh"
#include "ResBos/Pdf.hh"
#include "ResBos/ResBos.hh"
#include "ResBos/Settings.hh"
#include "ResBos/Utility.hh"
#include "ResBos/Vegas.hh"
#include "NonPertFit/Dataset.hh"

#include <algorithm>

namespace YAML {
template<typename T>
struct convert;
}

enum class ExpType {Collider, Fixed, Fixed2};

namespace ResBos {
    class ResBos;
    class Process;
}

class Experiment {
    public:
        Experiment();
        Experiment(const Experiment &other) {
            fit = other.fit;
            calculate = other.calculate;
            experiment = other.experiment;
            resbos = other.resbos;
            yResults = other.yResults;
            QMean = other.QMean;
            yMean = other.yMean;
            yk = other.yk;
            data = other.data;
            ptYInc = other.ptYInc;
        }
        Experiment &operator=(const Experiment &other) {
            fit = other.fit;
            calculate = other.calculate;
            experiment = other.experiment;
            resbos = other.resbos;
            yResults = other.yResults;
            QMean = other.QMean;
            yMean = other.yMean;
            yk = other.yk;
            data = other.data;
            ptYInc = other.ptYInc;

            return *this;
        }
        void Init();
        size_t Id() const { return data.ID(); }
        bool ToFit() const { return fit; }
        std::string Name() const { return data.Name(); }
        virtual ~Experiment() = default;
        void SetGees(std::vector<double> g) { 
            resbos -> GetCalculation() -> SetNonPertCoefficients(g); 
        }
        void SetBMax(double bMax) {//yfu
            resbos -> GetCalculation() ->SetBMax(bMax);
        }
        void SetYMax(double yMax) {//yfu
            resbos -> GetCalculation() ->SetYMax(yMax);
        }
        void ClearCache() {
            // std::map<std::pair<double, double>, double>().swap(QMean);
            // std::map<std::pair<double, double>, double>().swap(yMean);
            resbos -> GetCalculation() -> ClearMap();
        }
        double Chi2(bool=false, bool=false);
        size_t NPoints() const { return data.NPoints(); }

        friend struct YAML::convert<Experiment>;

        bool calculate{};

    private:
        double GetPrediction(size_t, bool=false);
        void CalculateCorrection(const std::function<std::vector<double>(double, double, double)> &,
                                 std::array<double, 2>&, double, double, double, double,
                                 size_t, bool=false) const;
        bool fit{}, yk{};
        ExpType experiment;
        DataSet data;
        double ptYInc;
        std::shared_ptr<ResBos::ResBos> resbos;
        std::unique_ptr<ResBos::Calculation> asym, pert;
        std::vector<double> yResults;
        std::vector<double> QMean{}, yMean{}, qtMean{};
        std::vector<std::array<double, 2>> correction;
};

#endif
