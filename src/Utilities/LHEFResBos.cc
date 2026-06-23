#include <sstream>

#include "LHEF/LHEFResBos.h"
#include "ResBos/Event.hh"
#include "ResBos/Particle.hh"

bool LHEF3FromResBos::OpenLHEF(std::string filename_) {
    // Open file for writing. Reset it to be empty.
    filename = filename_;
    osLHEF.open(filename.c_str(), std::ios::out | std::ios::trunc);
    if(!osLHEF)
        throw std::runtime_error("Could not open file" + filename);
    return true;
}

bool LHEF3FromResBos::CloseLHEF(bool updateInit) {
    // Write an end to the file.
    osLHEF << "</LesHouchesEvents>" << std::endl;
    osLHEF.close();

    // Optionally update the cross section information.
    if(updateInit) {
        osLHEF.open(filename.c_str(), std::ios::in | std::ios::out);

        SetInit();
        osLHEF.close();
    }

    return true;
}

bool LHEF3FromResBos::SetInit() {

    // Start wtih a clean writer
    writer.headerStream.str("");
    writer.initStream.str("");
    writer.headerStream.clear();
    writer.initStream.clear();

    // Set up arbitrary PDG id's of beam particles.
    // Right now just define as d and dbar
    heprup.IDBMUP = std::make_pair(1,-1);

    // Energy of beam particles given in GeV
    heprup.EBMUP = std::make_pair(event->BeamA(),event->BeamB());

    // The author group for the PDF used for the beams according to the
    // PDFLib specification
    heprup.PDFGUP = std::make_pair(0,0);

    // The id number the PDF used for the beams according to the
    // PDFLib specification
    heprup.PDFSUP = std::make_pair(0,0);

    // Master switch indicating how the ME generator envisages the
    // events weights. Should be interpreted according to the Les Houches accord.
    heprup.IDWTUP = -4; 

    // The number of different subprocesses in this file.
    heprup.NPRUP = 1;

    // The cross sections for the different subprocesses in pb.
    std::vector<double> XSECUP {event->XSec()};
    heprup.XSECUP = XSECUP;

    // The statistical error in the cross sections for the different
    // subprocesses in pb
    std::vector<double> XERRUP {event->XErr()};
    heprup.XERRUP = XERRUP;

    // The maximum event weights (in HEPEIP::XWGTUP) for different subprocesses
    std::vector<double> XMAXUP {0.0};
    heprup.XMAXUP = XMAXUP;

    // The subprocess code for the different subprocesses.
    std::vector<int> LPRUP {9999999};
    heprup.LPRUP = LPRUP;

    // Contents of the LHAinitrwgt tag

    // Contents of LHAgenerator tags

    // A map of the LHAweightgroup tags, indexed by name

    // A map of the LHAweight tags, indexed by name

    // Get init information.
    writer.version = 3;

    // Print ResBos settings
    std::string line;
    std::ifstream input("resbos.config");
    while(getline(input,line))
        writer.headerBlock() << line << '\n';

    writer.heprup = heprup;
    writer.init();

    return true;
}

bool LHEF3FromResBos::SetEvent(const Event& newEvent) {
    // Begin filling Les Houches blocks
    hepeup.clear();
    hepeup.resize(0);

    // The number of particle entries in the current event
    hepeup.NUP = 0;
    for(auto particle : newEvent.Particles()) {
        ++hepeup.NUP;
    }

    // The subprocess code for this newEvent (as given in LPRUP)
    hepeup.IDPRUP = 9999999;

    // The weight for this newEvent
    hepeup.XWGTUP = newEvent.XSec();

    // The PDF weights for the two incoming partons. Note that this
    // variable is not present in the current LesHouches accord
    // (<A HREF="http://arxiv.org/abs/hep-ph/0109068">hep-ph/0109068</A>),
    // hopefully it will be present in a future accord.
    hepeup.XPDWUP = std::make_pair(0,0);

    // The scale in GeV used in the calculation of the PDF's in this newEvent
    hepeup.SCALUP = newEvent.Scale();

    // The value of the QED coupling used in this newEvent
    hepeup.AQEDUP = newEvent.AlphaEM(); 

    // The value of the QCD coupling used in this newEvent
    hepeup.AQCDUP = newEvent.AlphaS();

    // Add all particles to the newEvent
    std::vector<double> p;
    for(size_t i = 0; i < newEvent.size(); ++i) {  
        hepeup.IDUP.push_back(newEvent[i].PID());
        hepeup.ISTUP.push_back(1);
        hepeup.MOTHUP.push_back(std::make_pair(newEvent[i].Mother1(),newEvent[i].Mother2()));
        hepeup.ICOLUP.push_back(newEvent[i].Cols());
        p = {newEvent[i].Px(),newEvent[i].Py(),newEvent[i].Pz(),newEvent[i].E(),newEvent[i].M()};
        hepeup.PUP.push_back(p);
        hepeup.VTIMUP.push_back(0);
        hepeup.SPINUP.push_back(9);
    }

    // A pointer to the current HEPRUP object
    hepeup.heprup = &heprup;

    // The weights associated with this newEvent, as given by the LHAwgt tags
    if(newEvent.WgtDetails()) hepeup.weights_detailed = *(newEvent.WgtDetails());

    // The weights associated with this newEvent, as given by the LHAweights tags
    if(newEvent.WgtCompressed()) hepeup.weights_compressed = *(newEvent.WgtCompressed());

    // Contents of the LHAscales tag
    if(newEvent.LHAScales()) hepeup.scales = *(newEvent.LHAScales());

    // Contents of the LHAweights tag (compressed format)
    if(newEvent.Weights()) hepeup.weights = *(newEvent.Weights());

    // Contents of the LHArwgt tag (detailed format)
    if(newEvent.RWeights()) hepeup.rwgt = *(newEvent.RWeights());

    // Any other attributes
    if(newEvent.EventAttributes()) hepeup.attributes = *(newEvent.EventAttributes());

    // Write the newEvent
    writer.hepeup = hepeup;
    writer.writeEvent(&hepeup);

    return true;
}
