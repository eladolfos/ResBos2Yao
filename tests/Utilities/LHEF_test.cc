#include "LHEF_test.hh"
#include "gtest/gtest.h"
#include <sstream>
#include <cstdio>

TEST(LHEF3Test, TestAll) {
    // Create and save test file
    std::string infileName = "wbj_lhef3.lhe";
    std::stringstream instream;
    std::ofstream infile(infileName);

    instream << "<LesHouchesEvents version=\"3.0\"> \n" 
        << "<header> \n"
        << "<initrwgt > \n"
        << "<weightgroup combine=\"envelope\" type=\"scale_variation\" > \n"
        << "<weight id=\"1001\" > muR=0.10000E+01 muF=0.10000E+01 </weight>\n"
        << "<weight id=\"1002\" > muR=0.10000E+01 muF=0.20000E+01 </weight>\n"
        << "<weight id=\"1003\" > muR=0.10000E+01 muF=0.50000E+00 </weight>\n"
        << "<weight id=\"1004\" > muR=0.20000E+01 muF=0.10000E+01 </weight>\n"
        << "<weight id=\"1005\" > muR=0.20000E+01 muF=0.20000E+01 </weight>\n"
        << "<weight id=\"1006\" > muR=0.20000E+01 muF=0.50000E+00 </weight>\n"
        << "<weight id=\"1007\" > muR=0.50000E+00 muF=0.10000E+01 </weight>\n"
        << "<weight id=\"1008\" > muR=0.50000E+00 muF=0.20000E+01 </weight>\n"
        << "<weight id=\"1009\" > muR=0.50000E+00 muF=0.50000E+00 </weight>\n"
        << "</weightgroup>\n"
        << "<weight id=\"1001\" > muR=0.10000E+01 muF=0.10000E+01 </weight>\n"
        << "<weight id=\"1002\" > muR=0.10000E+01 muF=0.20000E+01 </weight>\n"
        << "<weight id=\"1003\" > muR=0.10000E+01 muF=0.50000E+00 </weight>\n"
        << "<weight id=\"1004\" > muR=0.20000E+01 muF=0.10000E+01 </weight>\n"
        << "<weight id=\"1005\" > muR=0.20000E+01 muF=0.20000E+01 </weight>\n"
        << "<weight id=\"1006\" > muR=0.20000E+01 muF=0.50000E+00 </weight>\n"
        << "<weight id=\"1007\" > muR=0.50000E+00 muF=0.10000E+01 </weight>\n"
        << "<weight id=\"1008\" > muR=0.50000E+00 muF=0.20000E+01 </weight>\n"
        << "<weight id=\"1009\" > muR=0.50000E+00 muF=0.50000E+00 </weight>\n"
        << "</initrgwt>\n"
        << "</header>\n"
        << "<init> \n"
        << "2212     2212           4000           4000   -1   -1 21100 21100   -4    1 \n"
        << "50.109086    0.089185414      50.109093     66 \n"
        << "<generator name=\"SomeGen1\" version=\"1.2.3\" > some additional comments </generator>\n"
        << "</init>\n"
        << "<event npLO=\" -1 \" npNLO=\" 1 \"> \n"
        << "5     66      50.109093      141.37688   0.0075563862     0.12114027 \n"
        << "5 -1    0    0  501    0                  0                  0          143.22906          143.30946                4.8 0 0 \n"
        << "2 -1    0    0  502    0                  0                  0         -935.44317          935.44323               0.33 0 0 \n"
        << "24  1    1    2    0    0         -84.258804         -157.08566           -106.296          222.57162             80.398 0 0 \n"
        << "5  1    1    2  501    0         -136.68073         -36.307424         -40.614473          147.21558                4.8 0 0 \n"
        << "1  1    1    2  502    0          220.93954          193.39308         -645.30364          708.96548               0.33 0 0 \n"
        << "#aMCatNLO 1  6  2  0  0 0.00000000E+00 0.00000000E+00 9  0  0 0.10000000E+01 0.91292678E+00 0.10493312E+01 0.00000000E+00 0.00000000E+00 \n"
        << "#aMCatNLO 1  6  2  0  0 0.00000000E+00 0.00000000E+00 9  0  0 0.10000000E+01 0.91292678E+00 0.10493312E+01 0.00000000E+00 0.00000000E+00 \n"
        << "#aMCatNLO 1  6  2  0  0 0.00000000E+00 0.00000000E+00 9  0  0 0.10000000E+01 0.91292678E+00 0.10493312E+01 0.00000000E+00 0.00000000E+00\n"
        << "#aMCatNLO 1  6  2  0  0 0.00000000E+00 0.00000000E+00 9  0  0 0.10000000E+01 0.91292678E+00 0.10493312E+01 0.00000000E+00 0.00000000E+00 \n"
        << "#aMCatNLO 1  6  2  0  0 0.00000000E+00 0.00000000E+00 9  0  0 0.10000000E+01 0.91292678E+00 0.10493312E+01 0.00000000E+00 0.00000000E+00 \n"
        << "<rwgt > \n"
        << "<wgt id=\"1001\" >50.109</wgt>\n"
        << "<wgt id=\"1002\" >45.746</wgt>\n"
        << "<wgt id=\"1003\" >52.581</wgt>\n"
        << "<wgt id=\"1004\" >50.109</wgt>\n"
        << "<wgt id=\"1005\" >45.746</wgt>\n"
        << "<wgt id=\"1006\" >52.581</wgt>\n"
        << "<wgt id=\"1007\" >50.109</wgt>\n"
        << "<wgt id=\"1008\" >45.746</wgt>\n"
        << "<wgt id=\"1009\" >52.581</wgt>\n"
        << "</rgwt>\n"
        << "<weights > 1 0.204 1.564</weights>\n"
        << "<scales muf=\"90.1\" mur=\"90.2\" mups=\"90.3\" newscale=\"90.4\" random stuff </scales>\n"
        << "</event>\n"
        << "</LesHouchesEvents>\n";

    infile << instream.rdbuf();

    LHEF::Reader reader(infileName);

    std::stringstream outfile;
    LHEF::Writer writer(outfile);

    // Get init information
    writer.version = reader.version;
    writer.headerBlock() << reader.headerComments;
    writer.initComments() << reader.initComments;
    writer.heprup = reader.heprup;
    writer.init();

    // Read each event and write them out again
    while(reader.readEvent()) {
        writer.eventComments() << reader.eventComments;
        writer.hepeup = reader.hepeup;
        writer.writeEvent();
    }

    writer.print_end_tag();

    remove(infileName.c_str());
}
