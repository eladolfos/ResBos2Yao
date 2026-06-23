#ifndef PDF_H
#define PDF_H

#include <cmath>
#include <memory>
#include <mutex>

#include "LHAPDF/LHAPDF.h"
#include "LHAPDF/AlphaS.h"

namespace IO {
    class Settings;
}

namespace Utility {

    // Class for interfacing to LHAPDF, and ensuring thread safety
    class PDF{
        public:
            PDF() {pdf = NULL;}
            PDF(const PDF&);
            PDF(const IO::Settings *s);
            PDF(const std::string &pdfName, const int&, const IO::Settings *s);
            virtual ~PDF() {
                delete pdf;
                for(auto pdfi : pdfSet) delete pdfi;
            }

            virtual double Alphas(const double&);
            virtual double Alpi(const double& Q) { return Alphas(Q)/M_PI; }
            virtual double Alpi2(const double& Q) { return as.alphasQ(Q)/M_PI; }
            virtual double Apdf(const int&, const double&, const double&);
            virtual size_t NF(const double&);
            LHAPDF::PDF* GetPDF() const { return pdf; }
            LHAPDF::PDF* GetPDF(const size_t &i) const { return pdfSet[i]; }
            virtual size_t nPDFs() const { return lhaSet.size(); }
            LHAPDF::PDFUncertainty Uncertainty(const std::vector<double>&, const double&) const;
            virtual double qMin() const { return pdf -> qMin(); }
            virtual double qMax() const { return pdf -> qMax(); }
            virtual double xMin() const { return pdf -> xMin(); }
            virtual double xMax() const { return pdf -> xMax(); }
            LHAPDF::PDFInfo info() const { return pdf -> info(); }

            virtual int GetISet() const { return iSet; }
            virtual std::string GetName() const {return pdfName;}

        private:
            std::string pdfName;
            int iSet;
            LHAPDF::PDF* pdf;
            LHAPDF::PDFSet lhaSet;
            std::vector<LHAPDF::PDF*> pdfSet;
            LHAPDF::AlphaS_ODE as;
            std::mutex alphasMutex, pdfMutex, nfMutex;
            std::vector<double> masses;
    };
}

#endif
