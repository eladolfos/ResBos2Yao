#include "ResBos/FourVector.hh"
#include "ResBos/Lorentz.hh"
#include "ResBos/ThreeVector.hh"
#include "ResBos/Histogram.hh"

#include <iostream>
#include <cmath>

using namespace ResBos;
typedef HistogramCollection HC;

// All histograms desired for output should be defined here. Below are some basic examples
// For additional details, please see the ResBos2 Manual
void HC::InitializeHists() {
    // Create histograms for the pt of the electron and positron, with 100 bins from 0 to 100 GeV
    AddHistogram(100,0,100,"pt_e");
    AddHistogram(100,0,100,"pt_p");

    // Create a histogram for the transverse momentum of the intermediate boson
    // This is done using bins defined in a std::vector<double> 
    // You need to define all the bin edges (See manual for details)
    std::vector<double> qtBins = {
        0.0, 2.0, 4.0, 6.0, 8.0, 10., 12., 14., 16., 18.,
        20., 22., 24., 26., 28., 30., 35., 40., 45., 50.,
        55., 60., 70., 80., 90., 100, 120, 140, 160, 180,
        200, 250, 300, 350, 400, 500, 600
    };
    AddHistogram(qtBins,"qt");
    AddHistogram(qtBins,"qtNorm");

    std::vector<double> qtBins2 = {
        0.0,1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0,10.,
        11.,12.,13.,14.,15.,16.,17.,18.,19.,20.,21.,
        22.,23.,24.,25.,26.,27.,28.,29.,30.,31.,32.,
        33.,34.,35.,36.,37.,38.,39.,40.,45.,50.,55.,
        60.,65.,70.,75.,80.,85.,90.,95.,100
    };
    AddHistogram(qtBins2,"qt2");

    std::vector<double> cdf1 = {
        0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0,
        5.5, 6.0, 6.5, 7.0, 7.5, 8.0, 8.5, 9.0, 9.5, 10.0, 10.5,
        11.0, 11.5, 12.0, 13, 14, 15, 16, 17, 18, 19, 20
    };
    AddHistogram(cdf1, "CDF1");
    AddHistogram(40, 0, 20, "CDF2");

    std::vector<double> d01 = {
        0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 12.0, 14, 16, 18, 20
    };
    AddHistogram(d01, "D01");
    std::vector<double> d02 = {
        0.0, 2.2, 4.4, 8, 9.4, 13.2, 14.2, 18.2, 19.2
    };
    AddHistogram(d02, "D02");

    std::vector<double> atlas = {
        0.0, 2.0, 4.0, 6.0, 8.0, 10.0, 13.0, 16.0, 20.0
    };
    AddHistogram(atlas, "ATLAS04");
    AddHistogram(atlas, "ATLAS08");
    AddHistogram(atlas, "ATLAS12");
    AddHistogram(atlas, "ATLAS16");
    AddHistogram(atlas, "ATLAS20");
    AddHistogram(atlas, "ATLAS24");

    std::vector<double> e288 = {
        0.0, 0.2, 0.4, 0.6, 0.8, 1.0, 1.2, 1.4 
    };
    AddHistogram(e288, "e288200_45");
    AddHistogram(e288, "e288200_56");
    AddHistogram(e288, "e288200_67");
    AddHistogram(e288, "e288200_78");

    AddHistogram(e288, "e288300_45");
    AddHistogram(e288, "e288300_56");
    AddHistogram(e288, "e288300_67");
    AddHistogram(e288, "e288300_78");
    AddHistogram(e288, "e288300_1112");

    AddHistogram(e288, "e288400_56");
    AddHistogram(e288, "e288400_67");
    AddHistogram(e288, "e288400_78");
    AddHistogram(e288, "e288400_1112");
    AddHistogram(e288, "e288400_1213");
    AddHistogram(e288, "e288400_1314");

    AddHistogram(e288, "e605_78");
    AddHistogram(e288, "e605_89");
    AddHistogram(e288, "e605_1011");
    AddHistogram(e288, "e605_1114");
    AddHistogram(e288, "e605_1418");

    std::vector<double> r209 = {
        0.0, 0.4, 0.8, 1.2, 1.6, 2.0
    };
    AddHistogram(r209, "r209_58");
    AddHistogram(r209, "r209_811");

    std::vector<double> angularBins = {
        0.0, 2.5, 5.0, 8.0, 11.4, 14.9, 18.5, 22.0,
        25.5, 29.0, 32.6, 36.4, 40.4, 44.9, 50.2, 56.4, 
        63.9, 73.4, 85.4, 105.0, 132.0, 173.0, 253.0, 600.0
    };

    AddHistogram(angularBins, "pt");
    AddHistogram(angularBins, "A0");
    AddHistogram(angularBins, "A1");
    AddHistogram(angularBins, "A2");
    AddHistogram(angularBins, "A3");
    AddHistogram(angularBins, "A4");
    AddHistogram(angularBins, "A5");
    AddHistogram(angularBins, "A6");
    AddHistogram(angularBins, "A7");
}

constexpr int LeviCevita(const int i, const int j, const int k, const int l) {
    return (i==j||i==k||i==l||j==k||j==l||k==l) ? 0 : -(i-j)*(i-k)*(i-l)*(j-k)*(j-l)*(k-l)/12;
}

// Here all the histograms are filled. They can be accessed through the hists 
// map by the use of the above name.
// For additional details, please see the ResBos2 Manual
bool HC::FillHists(const std::vector<int>& pids, const std::vector<FourVector>& momenta, const double& wgt) {
    // Load the momentum into four vectors associated with the name of the particle
    FourVector electron = FourVector(), 
               positron = FourVector(), 
               Zboson = FourVector();
    for(size_t i = 0; i < pids.size(); ++i) {
        if(pids[i] == 11 || (pids[i] == 22 && i != 1)) electron = momenta[i];
        else if(pids[i] == -11 || pids[i] == 22) positron = momenta[i];
        else if(pids[i] == 24 || pids[i] == 24) Zboson = momenta[i];
    }

    // Calculate the transverse momentum of the electron, positron and Z, and fill the histograms
    hists["pt_e"] -> Fill(electron.Pt(), wgt); 
    hists["pt_p"] -> Fill(positron.Pt(), wgt); 
    hists["qt"] -> Fill(Zboson.Pt(), wgt);
    hists["qt2"] -> Fill(Zboson.Pt(), wgt);
    hists["CDF1"] -> Fill(Zboson.Pt(), wgt);
    hists["CDF2"] -> Fill(Zboson.Pt(), wgt);
    hists["D01"] -> Fill(Zboson.Pt(), wgt);
    hists["D02"] -> Fill(Zboson.Pt(), wgt);
    if(std::abs(Zboson.Rapidity()) < 0.4)
        hists["ATLAS04"] -> Fill(Zboson.Pt(), wgt);
    else if(std::abs(Zboson.Rapidity()) < 0.8)
        hists["ATLAS08"] -> Fill(Zboson.Pt(), wgt);
    else if(std::abs(Zboson.Rapidity()) < 1.2)
        hists["ATLAS12"] -> Fill(Zboson.Pt(), wgt);
    else if(std::abs(Zboson.Rapidity()) < 1.6)
        hists["ATLAS16"] -> Fill(Zboson.Pt(), wgt);
    else if(std::abs(Zboson.Rapidity()) < 2.0)
        hists["ATLAS20"] -> Fill(Zboson.Pt(), wgt);
    else if(std::abs(Zboson.Rapidity()) < 2.4)
        hists["ATLAS24"] -> Fill(Zboson.Pt(), wgt);
    // E288
    if(Zboson.M()  > 4.0 && Zboson.M() < 5.0) {
        hists["e288200_45"] -> Fill(Zboson.Pt(), wgt);
        hists["e288300_45"] -> Fill(Zboson.Pt(), wgt);
    } else if(Zboson.M() < 6.0) {
        hists["e288200_56"] -> Fill(Zboson.Pt(), wgt);
        hists["e288300_56"] -> Fill(Zboson.Pt(), wgt);
        hists["e288400_56"] -> Fill(Zboson.Pt(), wgt);
    } else if(Zboson.M() < 7.0) {
        hists["e288200_67"] -> Fill(Zboson.Pt(), wgt);
        hists["e288300_67"] -> Fill(Zboson.Pt(), wgt);
        hists["e288400_67"] -> Fill(Zboson.Pt(), wgt);
    } else if(Zboson.M() < 8.0) {
        hists["e288200_78"] -> Fill(Zboson.Pt(), wgt);
        hists["e288300_78"] -> Fill(Zboson.Pt(), wgt);
        hists["e288400_78"] -> Fill(Zboson.Pt(), wgt);
    } else if(Zboson.M() > 11 && Zboson.M() < 12) {
        hists["e288300_1112"] -> Fill(Zboson.Pt(), wgt);
        hists["e288400_1112"] -> Fill(Zboson.Pt(), wgt);
    } else if(Zboson.M() < 13) {
        hists["e288400_1213"] -> Fill(Zboson.Pt(), wgt);
    } else if(Zboson.M() < 14) {
        hists["e288400_1314"] -> Fill(Zboson.Pt(), wgt);
    }
    // E605
    if(Zboson.M() > 7.0 && Zboson.M() < 8.0) {
        hists["e605_78"] -> Fill(Zboson.Pt(), wgt);
    } else if(Zboson.M() > 8.0 && Zboson.M() < 9.0) {
        hists["e605_89"] -> Fill(Zboson.Pt(), wgt);
    } else if(Zboson.M() > 10.5 && Zboson.M() < 11.5) {
        hists["e605_1011"] -> Fill(Zboson.Pt(), wgt);
    } else if(Zboson.M() > 11.5 && Zboson.M() < 13.5) {
        hists["e605_1114"] -> Fill(Zboson.Pt(), wgt);
    } else if(Zboson.M() > 13.5 && Zboson.M() < 18) {
        hists["e605_1418"] -> Fill(Zboson.Pt(), wgt);
    } 
    // R209
    if(Zboson.M() > 5.0 && Zboson.M() < 8.0) {
        hists["r209_58"] -> Fill(Zboson.Pt(), wgt);
    } else if(Zboson.M() > 8.0 && Zboson.M() < 11) {
        hists["r209_811"] -> Fill(Zboson.Pt(), wgt);
    }

    // Angular calculation
    /*
    double sign = std::abs(Zboson.Pz())/Zboson.Pz();
    FourVector p1(0, 0, sign*4000, 4000);
    FourVector p2(0, 0, -sign*4000, 4000);
    ThreeVector CS = -Zboson.BoostVector();
    FourVector boostedE = electron;
    FourVector boostedP = positron;
    FourVector boostedZ = Zboson;
    boostedE.Boost(CS);
    boostedP.Boost(CS);
    boostedZ.Boost(CS);
    p1.Boost(CS);
    p2.Boost(CS);
    ThreeVector CSAxis = (p1.GetVect().Unit() - p2.GetVect().Unit()).Unit();
    ThreeVector yAxis = (p1.GetVect().Unit().Cross(p2.GetVect().Unit())).Unit();
    ThreeVector xAxis = -yAxis.Cross(CSAxis).Unit();
    */
    
    double Q = Zboson.M();
    double MT = sqrt(Zboson.M2() + Zboson.Pt2());
    double qp = (Zboson.E() + Zboson.Pz())/sqrt(2);
    double qm = (Zboson.E() - Zboson.Pz())/sqrt(2);
    FourVector n(0, 0, 1, 1);
    n /= sqrt(2);
    FourVector nb(0, 0, -1, 1);
    nb /= sqrt(2);
    const FourVector CSAxis = (qp*n - qm*nb)/MT;
    const FourVector xAxis = -Q/(Zboson.Pt()*MT)*(qp*n+qm*nb-MT*MT/Q/Q*Zboson);
    FourVector yAxis = {0,0,0,0};
    for(int i = 0; i < 4; i++){
         for(int j = 0; j < 4; j++) {
             for(int k = 0; k < 4; k++) {
                 for(int l = 0; l < 4; l++) {
                     int LC = LeviCevita(i,j,k,l);
                     if(LC==0) continue;
                     yAxis[l]+=Zboson[i]*xAxis[j]*CSAxis[k]/Q*LC;
                 }
             }
         }
    }

    yAxis[3] = -yAxis[3];

    // const double costheta = std::cos(boostedE.GetVect().Angle(CSAxis));
    // const double sintheta = 1.0-costheta*costheta;
    // double phi = std::atan2(boostedE.GetVect().Dot(yAxis),
    //                         boostedE.GetVect().Dot(xAxis));
    // if(phi < 0) phi += 2*M_PI;
    
    double m = Zboson.M();
    double mT = sqrt(m*m + Zboson.Pt2());
    double sign = Zboson.Pz()/std::abs(Zboson.Pz());
    double eplus = (electron.E() + electron.Pz())/sqrt(2.0);
    double eminus = (electron.E() - electron.Pz())/sqrt(2.0);
    double pplus = (positron.E() + positron.Pz())/sqrt(2.0);
    double pminus = (positron.E() - positron.Pz())/sqrt(2.0);

    double costheta = sign*2.0*(eplus*pminus-eminus*pplus);
    costheta /= (m*mT);
    double sintheta = sqrt(1.0 - costheta*costheta);

    double cosphi = 2.0*(eplus*eminus-pplus*pminus);
    cosphi /= (Zboson.Pt()*mT*sintheta);

    double pDotY = electron*yAxis;
    double sinphi = -2/(m*sintheta)*sign*pDotY;

    double A0 = (10.0/3.0*(1.0-3.0*costheta*costheta)+2.0/3.0)*wgt;
    double A1 = 10.0*sintheta*costheta*cosphi*wgt;
    double A2 = 10.0*sintheta*sintheta*(cosphi*cosphi-sinphi*sinphi)*wgt;
    double A3 = 4.0*sintheta*cosphi*wgt;
    double A4 = 4.0*costheta*wgt;
    double A5 = 5.0*sintheta*sintheta*2*sinphi*cosphi*wgt;
    double A6 = 10.0*sintheta*costheta*sinphi*wgt;
    double A7 = 4.0*sintheta*sinphi*wgt;
    hists["pt"] -> Fill(Zboson.Pt(), wgt);
    hists["A0"] -> Fill(Zboson.Pt(), A0);
    hists["A1"] -> Fill(Zboson.Pt(), A1);
    hists["A2"] -> Fill(Zboson.Pt(), A2);
    hists["A3"] -> Fill(Zboson.Pt(), A3);
    hists["A4"] -> Fill(Zboson.Pt(), A4);
    hists["A5"] -> Fill(Zboson.Pt(), A5);
    hists["A6"] -> Fill(Zboson.Pt(), A6);
    hists["A7"] -> Fill(Zboson.Pt(), A7);

    // Transverse momentum can also be calculated from the electron and positron
    hists["qtNorm"] -> Fill((electron+positron).Pt(), wgt);

    // This is a flag to let the ResBos code know if the filling was preformed successfully.
    return true;
}

// At the end, if you wish to preform any actions on the histogram as a whole, that can be preformed here.
// For additional details, please see the ResBos2 Manual
bool HC::Finalize() {
    // Here we normalize the qtNorm histogram to 1
    hists["qtNorm"] -> Normalize();


    hists["A0"] -> Ratio(*hists["pt"]);
    hists["A1"] -> Ratio(*hists["pt"]);
    hists["A2"] -> Ratio(*hists["pt"]);
    hists["A3"] -> Ratio(*hists["pt"]);
    hists["A4"] -> Ratio(*hists["pt"]);
    hists["A5"] -> Ratio(*hists["pt"]);
    hists["A6"] -> Ratio(*hists["pt"]);
    hists["A7"] -> Ratio(*hists["pt"]);

    // This is a flag to let the ResBos code know if the finalization was preformed successfully.
    return true;
}
