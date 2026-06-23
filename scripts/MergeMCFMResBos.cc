#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <cmath>

#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TLorentzVector.h"

double kFac(double y, double pt) {
    return 1.06545+0.626109/(pt*pt)+0.0106729*cosh(y)-0.0613369*cosh(y)/(pt*pt)-3.62659*log(pt*pt)/(pt*pt)+0.137002*cosh(y)*log(pt*pt)/(pt*pt);
}

int main(int argv, char* argc[]) {
    if(argv < 4) {
        std::cout << "Usage: ./MergeMCFMResBos qtCut MCFM.root ResBos.root NNLO/NLO/LO" << std::endl;
        return -1;
    }

    double qtCut = atof(argc[1]);
    std::string MCFMFile = std::string(argc[2]);
    std::string ResBosFile = std::string(argc[3]);
    std::string Order = std::string(argc[4]);
    std::cout << argc[1] << "\t" << argc[2] << "\t" << argc[3] << "\t" << argc[4] << std::endl;
    std::cout << Order << std::endl;
    if(Order.compare("NNLO") != 0 && Order.compare("NLO") != 0 && Order.compare("LO") != 0) {
        std::cout << "Order requested is not valid. Only works for Z+jet calculation at LO, NLO, or NNLO." << std::endl;
        std::cout << "Usage: ./MergeMCFMResBos qtCut MCFM.root ResBos.root NNLO/NLO/LO" << std::endl;
        return -1;
    }

    float p3[4], p4[4], wt;

    TFile* MCFM = new TFile(MCFMFile.c_str(),"READONLY");
    TTree* tree = (TTree*) MCFM->Get("h10");

    tree -> SetBranchAddress("px3",&p3[0]);
    tree -> SetBranchAddress("py3",&p3[1]);
    tree -> SetBranchAddress("pz3",&p3[2]);
    tree -> SetBranchAddress("E_3",&p3[3]);
    tree -> SetBranchAddress("px4",&p4[0]);
    tree -> SetBranchAddress("py4",&p4[1]);
    tree -> SetBranchAddress("pz4",&p4[2]);
    tree -> SetBranchAddress("E_4",&p4[3]);
    tree -> SetBranchAddress("wt_ALL",&wt);

    double ptBins[44] = {0,2,4,6,8,10,12,14,16,18,20,22.5,25.0,27.5,30,33,36,39,42,45,48,51,54,57,61,65,70,75,80,85,95,105,125,150,175,200,250,300,350,400,470,550,650,900};

    TFile* Output = new TFile("Results.root","RECREATE");
    TH1F* qtMCFM = new TH1F("qtMCFM","qtMCFM",43,ptBins);
    TH1F* qtResBos = new TH1F("qtResBos","qtResBos",43,ptBins);
    TH1F* qtTot = new TH1F("qt","qt",43,ptBins);
    TH1F* qtCutTest = new TH1F("qtTest","qtTest",1800,0,900);

    double pt, y, Mll2, ptL1, ptL2, yL1, yL2;
    for(int i = 0; i < tree -> GetEntries(); i++) {
        tree -> GetEntry(i);
        pt = sqrt(pow(p3[0]+p4[0],2)+pow(p3[1]+p4[1],2));
        y = 0.5*log((p3[3]+p4[3]+p3[2]+p4[2])/(p3[3]+p4[3]-(p3[2]+p4[2])));
        Mll2 = -(pow(p3[0]+p4[0],2)+pow(p3[1]+p4[1],2)+pow(p3[2]+p4[2],2))+pow(p3[3]+p4[3],2);
        ptL1 = sqrt(pow(p3[0],2)+pow(p3[1],2));
        ptL2 = sqrt(pow(p4[0],2)+pow(p4[1],2));
        yL1 = 0.5*log((p3[3]+p3[2])/(p3[3]-p3[2]));
        yL2 = 0.5*log((p4[3]+p4[2])/(p4[3]-p4[2]));
        if(ptL1 < 20 || ptL2 < 20) continue;
        if(fabs(yL1) > 2.4 || fabs(yL2) > 2.4) continue;
        if(fabs(y) > 2.4) continue;
        if(Mll2 < 66*66 || Mll2 > 116*116) continue;
        qtCutTest -> Fill(pt,wt/1000.0*2);
        if(pt < qtCut) continue;
        qtMCFM -> Fill(pt,wt/1000.0/qtMCFM -> GetBinWidth(qtMCFM -> FindBin(pt)));
    }

    TFile* ResBos = new TFile(ResBosFile.c_str(),"READONLY");
    TTree* ResBosTree = (TTree*) ResBos->Get("h10");
    if(ResBosTree == NULL) {
        std::cout << "Tree does not exists!!" << std::endl;
        return -1;
    }
    float pT_B, WT00, mll, ptl1, ptl2, yl1, yl2, yb;
   
    ResBosTree -> SetBranchAddress("pT_d1",&ptl1);
    ResBosTree -> SetBranchAddress("pT_d2",&ptl2);
    ResBosTree -> SetBranchAddress("y_d1",&yl1);
    ResBosTree -> SetBranchAddress("y_d2",&yl2);
    ResBosTree -> SetBranchAddress("pT_B",&pT_B);
    ResBosTree -> SetBranchAddress("M_B",&mll);
    ResBosTree -> SetBranchAddress("y_B",&yb);
    ResBosTree -> SetBranchAddress("WT00",&WT00);

    for(int i = 0; i < ResBosTree -> GetEntries(); i++) {
        ResBosTree -> GetEntry(i);
        if(ptl1 < 20 || ptl2 < 20) continue;
        if(fabs(yl1) > 2.4 || fabs(yl2) > 2.4) continue;
        if(fabs(yb) > 2.4) continue;
        if(mll < 66 || mll > 116) continue;
        if(pT_B > 22) continue;
        qtResBos -> Fill(pT_B,WT00/qtResBos -> GetBinWidth(qtResBos -> FindBin(pT_B)));
    }

    Output -> cd();
    if(Order.compare("NNLO") == 0) {
        double kFac[43] = {0.3479,0.7041,0.8301,0.8959,0.9479,0.9616,1.002,1.014,1.030,1.049,1.052,1.060,1.058,1.063,1.068,1.068,1.055,1.058,1.082,1.068,1.079,1.060,1.085,1.074,1.077,1.079,1.079,1.079,1.082,1.077,1.074,1.077,1.079,1.082,1.077,1.077,1.077,1.085,1.079,1.085,1.085,1.090};
        for(int i = 1; i <= 43; i++) {
            qtMCFM -> SetBinContent(i,kFac[i-1]*qtMCFM -> GetBinContent(i));
        }
    }
    qtMCFM -> Scale(1.0/qtMCFM -> Integral("width"));
    qtResBos -> Scale(1.0/qtResBos -> Integral("width"));
    qtTot -> Add(qtMCFM);
    qtTot -> Add(qtResBos);
    qtTot -> Draw();

    Output -> Write();


    return 0;
}

