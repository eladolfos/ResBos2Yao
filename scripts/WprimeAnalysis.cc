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
    if(argc != 4) {
        std::cout << "Usage: resum.root asym.root pert.root" << std::endl;
        return -1;
    }

    TString resumName = argv[1];
    TString asymName = argv[2];
    TString pertName = argv[3];

    TFile* resum = new TFile(argv[1],"READ");
    if(resum == NULL) error("Can't open " + resumName + ". Please make sure it exists", badFile);
    TFile* asym  = new TFile(argv[2],"READ");
    if(asym == NULL) error("Can't open " + asymName + ". Please make sure it exists", badFile);
    TFile* pert  = new TFile(argv[3],"READ");
    if(pert == NULL) error("Can't open " + pertName + ". Please make sure it exists", badFile);

    TTree* tResum = (TTree*) resum -> Get("Events");
    if(tResum == NULL) error("No Tree: resum. Please make sure it exists", badTree);
    TTree* tAsym  = (TTree*) asym  -> Get("Events");
    if(tAsym == NULL) error("No Tree: asym. Please make sure it exists", badTree);
    TTree* tPert  = (TTree*) pert  -> Get("Events");
    if(tPert == NULL) error("No Tree: pert. Please make sure it exists", badTree);

    TFile* merged = new TFile("merged.root","RECREATE");
    std::vector<TH1D*> pResum, pAsym, pPert;

    std::cout << "Reading in resummed events and creating plots..." << std::flush;
    pResum = ReadIn(tResum,1);
    std::cout << " done" << std::endl;

    std::cout << "Reading in asymptotic events and creating plots..." << std::flush;
    pAsym = ReadIn(tAsym,2);
    std::cout << " done" << std::endl;

    std::cout << "Reading in perturbative events and creating plots..." << std::flush;
    pPert = ReadIn(tPert,3);
    std::cout << " done" << std::endl;

    std::cout << "Merging plots and saving...";
    std::vector<TH1D*> pTot(pResum.size());
    for(unsigned i = 0; i < pResum.size(); i++){
        pTot[i] = (TH1D*) pResum[i] -> Clone();
        const char* name = pTot[i] -> GetName();
        size_t length = strlen(name)-1;
        char newname[20];
        strncpy(newname,name,length);
        pTot[i] -> SetName(newname);
        pTot[i] -> Add(pPert[i]);
        pTot[i] -> Add(pAsym[i],-1);
        pResum[i] -> Write();
        pPert[i] -> Write();
        pAsym[i] -> Write();
        pTot[i] -> Write();
        memset(&newname[0], 0, sizeof(newname));
    }
    std::cout << " done" << std::endl;

    merged -> Close();

    return 0;
}

std::vector<TH1D*> ReadIn(TTree* tree, int mode) {
    std::vector<TH1D*> plots;
    double qtCut = 4;

    char *QT = new char[10];
    char *pTj = new char[10];
    char *pTt = new char[10];
    char *Mtb = new char[10];
    char *yj = new char[10];
    char *yt = new char[10];
    char *DPhi = new char[10];

    sprintf(QT, "qt%d", mode);
    sprintf(pTj, "pTj%d", mode);
    sprintf(pTt, "pTt%d", mode);
    sprintf(Mtb, "Mtb%d", mode);
    sprintf(yj, "yj%d", mode);
    sprintf(yt, "yt%d", mode);
    sprintf(DPhi, "hDPhi%d", mode);

    TH1D* hQT = new TH1D(QT,"qt",300,0,300);
    TH1D* hpTj = new TH1D(pTj,"pTj", 300,0,3000);
    TH1D* hpTt = new TH1D(pTt,"pTt", 300,0,3000);
    TH1D* hMtb = new TH1D(Mtb,"Mtb", 1000,2000,6000);
    TH1D* hYj = new TH1D(yj,"yj",60,-6,6);
    TH1D* hYt = new TH1D(yt,"yh",60,-6,6);
    TH1D* hDphi = new TH1D(DPhi,"DPhi",30,0,TMath::Pi());

    TLorentzVector *tp = new TLorentzVector();
    TLorentzVector *jp = new TLorentzVector();
    double wtReal, wtResum, wtAsym;

    tree -> SetBranchAddress("Top",&tp);
    tree -> SetBranchAddress("Bottom",&jp);
    tree -> SetBranchAddress("wtReal", &wtReal);
    tree -> SetBranchAddress("wtResum", &wtResum);
    tree -> SetBranchAddress("wtAsym", &wtAsym);

    int nentries = tree -> GetEntries();

    for(int i = 0; i < nentries; i++) {
        tree -> GetEntry(i);

        TLorentzVector tot = *tp + *jp;
        if((mode == 3 || mode == 2) && tot.Pt() < qtCut) continue;
        double wt = 0;
        switch (mode){
            case 1:
                wt = wtResum;
                break;
            case 2:
                wt = wtAsym;
                break;
            case 3:
                wt = wtReal;
                break;
        }

        hQT -> Fill(tot.Pt(),wt);
        hpTj -> Fill(jp->Pt(),wt);
        hpTt -> Fill(tp->Pt(),wt);
        hMtb -> Fill(tot.M(),wt);
        hYj -> Fill(jp->Rapidity(),wt);
        hYt -> Fill(tp->Rapidity(),wt);
        hDphi -> Fill(jp->DeltaPhi(*tp),wt);
    }

    plots.push_back(hQT);
    plots.push_back(hpTj);
    plots.push_back(hpTt);
    plots.push_back(hMtb);
    plots.push_back(hYj);
    plots.push_back(hYt);
    plots.push_back(hDphi);

    for(unsigned i = 0; i < plots.size(); i++) {
        for(int j = 1; j <= plots[i]->GetNbinsX(); j++) {
            plots[i] -> SetBinContent(j, plots[i] -> GetBinContent(j)/plots[i] -> GetBinWidth(j));
        }
    }

    return plots;
}
