#include "ResBos/Pdf.hh"
#include "ResBos/Settings.hh"

namespace Utility {

    // Initialize the PDF for the calculation given the input settings
    PDF::PDF(const IO::Settings *s) {
        // Get the name of the PDF from the settings
        pdfName = s->GetSettingString("PDF");
        // Get the set number from the settings
        iSet = s->GetSettingInt("iSet");
        // If it is less than 0, run over central PDF and all error sets, else use only given set
        LHAPDF::PDFSet PDFSet(pdfName);
        lhaSet = PDFSet;
        if(iSet >= 0) {
            pdf = PDFSet.mkPDF(iSet);
        } else {
            pdfSet = PDFSet.mkPDFs();
        }

        // Set up alphas ode based on PDF to get number of active flavors
        as.setOrderQCD(4);
        as.setMZ(s->GetSettingDouble("mz"));
        as.setAlphaSMZ(Alphas(s->GetSettingDouble("mz")));
        as.setQuarkMass(1, s->GetSettingDouble("md"));
        as.setQuarkMass(2, s->GetSettingDouble("mu"));
        as.setQuarkMass(3, s->GetSettingDouble("ms"));
        as.setQuarkMass(4, s->GetSettingDouble("mc"));
        as.setQuarkMass(5, s->GetSettingDouble("mb"));
        as.setQuarkMass(6, s->GetSettingDouble("mt"));
        as.setFlavorScheme(LHAPDF::AlphaS::VARIABLE,5);
        masses = {s->GetSettingDouble("md"),
                  s->GetSettingDouble("mu"),
                  s->GetSettingDouble("ms"),
                  s->GetSettingDouble("mc"),
                  s->GetSettingDouble("mb")};
    }

    // Initialize the PDF for the calculation for a given set and set index
    PDF::PDF(const std::string &pdfName_, const int &iSet_, const IO::Settings *s) 
        : pdfName(pdfName_), iSet(iSet_) {

        LHAPDF::PDFSet PDFSet(pdfName);
        lhaSet = PDFSet;
        pdf = PDFSet.mkPDF(iSet);

        // Set up alphas ode based on PDF to get number of active flavors
        as.setOrderQCD(4);
        as.setMZ(s->GetSettingDouble("mz"));
        as.setAlphaSMZ(Alphas(s->GetSettingDouble("mz")));
        as.setQuarkMass(1, s->GetSettingDouble("md"));
        as.setQuarkMass(2, s->GetSettingDouble("mu"));
        as.setQuarkMass(3, s->GetSettingDouble("ms"));
        as.setQuarkMass(4, s->GetSettingDouble("mc"));
        as.setQuarkMass(5, s->GetSettingDouble("mb"));
        as.setQuarkMass(6, s->GetSettingDouble("mt"));
        as.setFlavorScheme(LHAPDF::AlphaS::VARIABLE,5);
        masses = {s->GetSettingDouble("md"),
                  s->GetSettingDouble("mu"),
                  s->GetSettingDouble("ms"),
                  s->GetSettingDouble("mc"),
                  s->GetSettingDouble("mb")};
    }

    PDF::PDF(const PDF& other) {
        pdfName = other.pdfName;
        iSet = other.iSet;
        lhaSet = other.lhaSet;
        pdf = other.pdf;
        as = other.as;
    }

    // Return the alphas value, ensuring that it is accesssed only on one thread
    double PDF::Alphas(const double &Q) {
        return pdf -> alphasQ2(Q*Q);
    }

    // Return the pdf value, ensuring that it is accesssed only on one thread
    double PDF::Apdf(const int &pid, const double &x, const double &Q) {
        if(x > 1) return 0;
        return pdf->xfxQ2(pid,x,Q*Q)/x;
    }

    // TODO:
    //      - Implement the return of the alphas and pdf value for multiple sets if running in that mode
    //      - Implement the return of a map to all of the parton flavors

    // Return the nf value, ensuring that it is accesssed only on one thread
    size_t PDF::NF(const double &Q) {
//        std::lock_guard<std::mutex> lock(nfMutex);
        return std::distance(masses.begin(), std::lower_bound(masses.begin(), masses.end(), Q));
    }

    LHAPDF::PDFUncertainty PDF::Uncertainty(const std::vector<double> &data,
                                            const double &CL) const {
        return lhaSet.uncertainty(data,CL);
    }

}
