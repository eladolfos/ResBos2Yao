#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TLorentzVector.h"

int main(int argv, char* argc[]) {
    if(argv < 2) {
        std::cout << "Usage: ./MCFMtoRoot infile (outfile)" << std::endl;
        return -1;
    }

    std::string infile = std::string(argc[1]);
    std::string outfile;
    double p3[4], p4[4], wt;

    if(argv == 3) {
        outfile = std::string(argc[2]);
    } else {
        outfile = infile.size();
    }

    infile = infile+".txt";
    outfile = outfile+".root";
    std::ifstream input(infile.c_str());
    TFile* output = new TFile(outfile.c_str(),"RECREATE");
    TTree* tree = new TTree("Events","Events");
    TH1F* qt = new TH1F("qt","qt",900,0,900);

    tree -> Branch("p3x",&p3[0]);
    tree -> Branch("p3y",&p3[1]);
    tree -> Branch("p3z",&p3[2]);
    tree -> Branch("E3",&p3[3]);
    tree -> Branch("p4x",&p4[0]);
    tree -> Branch("p4y",&p4[1]);
    tree -> Branch("p4z",&p4[2]);
    tree -> Branch("E4",&p4[3]);
    tree -> Branch("wt",&wt);

    while(input >> p3[0] >> p3[1] >> p3[2] >> p3[3] >> p4[0] >> p4[1] >> p4[2] >> p4[3] >> wt) {
        tree -> Fill();
        double pt = sqrt(pow(p3[0]+p4[0],2)+pow(p3[1]+p3[1],2));
        qt -> Fill(pt,wt);
    }

    tree -> Write();
    qt -> Write();
    output -> Close();

    return 0;
}
