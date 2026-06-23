#include <iostream>
#include <string>
#include <random>

#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TLorentzVector.h"

enum errorList {badFile, badTree};
std::random_device r; 
std::mt19937 e(r());

void error(TString msg, errorList errCode) {
    std::cerr << msg << std::endl;
    exit(errCode);
}

std::vector<TH1D*> ReadIn(TTree* tree, int mode);

int main(int argc, char** argv) {
    if(argc != 2) {
        std::cout << "Usage: pert.root" << std::endl;
        return -1;
    }

    TString pertName = argv[1];

    TFile* pert  = new TFile(argv[1],"READ");
    if(pert == NULL) error("Can't open " + pertName + ". Please make sure it exists", badFile);

    TTree* tPert  = (TTree*) pert  -> Get("h10");
    if(tPert == NULL) error("No Tree: pert. Please make sure it exists", badTree);

    TFile* merged = new TFile("ZjNLO.root","RECREATE");
    std::vector<TH1D*> pPert;

    std::cout << "Reading in perturbative events and creating plots..." << std::flush;
    pPert = ReadIn(tPert,3);
    std::cout << " done" << std::endl;

    std::vector<TH1D*> pTot(pPert.size());
    for(unsigned i = 0; i < pPert.size(); i++){
        pPert[i] -> Write();
    }

    merged -> Close();

    return 0;
}

std::vector<TH1D*> ReadIn(TTree* tree, int mode) {
    std::vector<TLorentzVector> photon(2);
    std::vector<TH1D*> plots;

    char *QT = new char[10];

    sprintf(QT, "qT%d", mode);

    double qTBins[44] = {0,2,4,6,8,10,12,14,16,18,20,22.5,25,27.5,30,33,36,39,42,45,48,51,54,57,61,65,70,75,80,85,95,105,125,150,175,200,250,300,350,400,470,550,
        650,900};

    TH1D* hQT = new TH1D(QT,"qt",43,qTBins);

    TLorentzVector h, l, lb;
    float wtall, px3, py3, pz3, E3, px4, py4, pz4, E4, px5, py5, pz5, E5;
    float px6, py6, pz6, E6;

    tree -> SetBranchAddress("px3",&px3);
    tree -> SetBranchAddress("py3",&py3);
    tree -> SetBranchAddress("pz3",&pz3);
    tree -> SetBranchAddress("E_3", &E3);
    tree -> SetBranchAddress("px4",&px4);
    tree -> SetBranchAddress("py4",&py4);
    tree -> SetBranchAddress("pz4",&pz4);
    tree -> SetBranchAddress("E_4", &E4);
    tree -> SetBranchAddress("px5",&px5);
    tree -> SetBranchAddress("py5",&py5);
    tree -> SetBranchAddress("pz5",&pz5);
    tree -> SetBranchAddress("E_5", &E5);
    tree -> SetBranchAddress("px6",&px6);
    tree -> SetBranchAddress("py6",&py6);
    tree -> SetBranchAddress("pz6",&pz6);
    tree -> SetBranchAddress("E_6", &E6);
    tree -> SetBranchAddress("wt_ALL",&wtall);

    int nentries = tree -> GetEntries();

    for(int i = 0; i < nentries; i++) {
        tree -> GetEntry(i);

        l.SetPxPyPzE(px3,py3,pz3,E3);
        lb.SetPxPyPzE(px4,py4,pz4,E4);
        h.SetPxPyPzE(px3+px4,py3+py4,pz3+pz4,E3+E4);
//        if(h.Pt()>2) continue;
        if(l.Pt()<20||lb.Pt()<20) continue;
        if(h.M() < 66 || h.M() > 116) continue;
        if(fabs(l.Rapidity()) > 2.4) continue;
        if(fabs(lb.Rapidity()) > 2.4) continue;
        if(fabs(h.Rapidity()) > 2.4) continue;

        hQT -> Fill(h.Pt(),wtall);
    }


    plots.push_back(hQT);

    for(unsigned i = 0; i < plots.size(); i++) {
        for(int j = 1; j <= plots[i]->GetNbinsX(); j++) {
            plots[i] -> SetBinContent(j, plots[i] -> GetBinContent(j)/plots[i] -> GetBinWidth(j));
            std::cout << plots[i] -> GetBinCenter(j) << "\t" << plots[i] -> GetBinContent(j) << std::endl;
        }
    }

    return plots;
}
