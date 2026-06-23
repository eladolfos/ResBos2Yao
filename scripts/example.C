#include "TString.h"
#include "TSystem.h"
#include "TFile.h"

void example(TString filename="../resbos.root") {
    gSystem -> Load("../lib/libParticle.so");
    TFile *f = new TFile(filename,"UPDATE");
}
