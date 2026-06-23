// This file (c) Leif Lonnblad and Stefan Prestel
// LHEF3.h contains the main class for LHEF 3.0 functionalities.
// Function definitions.

#include "LHEF/LHEF3.h"

namespace LHEF {

    //==========================================================================

    // The XMLTag struct is used to represent all information within an XML tag. 
    // It contains the attributes as a map, any sub-tags as a vector of pointers 
    // to other XMLTag objects, and any other information as a single string.

    //==========================================================================

    // The LHAweights struct.

    //--------------------------------------------------------------------------

    // Construct from XML tag.

    LHAweights::LHAweights(const XMLTag & tag) {
        for ( std::map<std::string,std::string>::const_iterator it = tag.attr.begin();
                it != tag.attr.end(); ++it ) {
            std::string v = it->second.c_str();
            attributes[it->first] = v;
        }

        contents = tag.contents;

        std::istringstream iss(tag.contents);
        double w;
        while ( iss >> w ) weights.push_back(w);
    }

    //--------------------------------------------------------------------------

    // Print out.

    void LHAweights::print(std::ostream & file) const {
        file << "<weights";
        for ( std::map<std::string,std::string>::const_iterator it = attributes.begin();
                it != attributes.end(); ++it )
            file << " " << it->first << "=\"" << it->second << "\"";
        file << " >";
        for ( size_t j = 0, M = weights.size(); j < M; ++j ) file << " " << weights[j];
        file << "</weights>" << std::endl;
    }

    //==========================================================================

    // The LHAscales struct: Collect different scales relevant for an event.

    //--------------------------------------------------------------------------

    // Construct from an XML-tag.

    LHAscales::LHAscales(const XMLTag & tag, double defscale)
        : muf(defscale), mur(defscale), mups(defscale), SCALUP(defscale) {
            for ( std::map<std::string,std::string>::const_iterator it = tag.attr.begin();
                    it != tag.attr.end(); ++it ) {
                double v = atof(it->second.c_str());
                if ( it->first == "muf" ) muf = v;
                else if ( it->first == "mur" ) mur = v;
                else if ( it->first == "mups" ) mups = v;
                else attributes[it->first] = v;
            }
            contents = tag.contents;
        }

    //--------------------------------------------------------------------------

    // Print out the corresponding XML-tag.

    void LHAscales::print(std::ostream & file) const {
        file << "<scales";
        file << " muf=\"" << muf << "\"";
        file << " mur=\"" << mur << "\"";
        file << " mups=\"" << mups << "\"";
        for ( std::map<std::string,double>::const_iterator it = attributes.begin();
                it != attributes.end(); ++it )
            file << " " << it->first << "=\"" << it->second << "\"";
        file << contents;
        file << "</scales>" << std::endl;
    }

    //==========================================================================

    // The LHAgenerator struct: Collect generator information for an event file.

    //--------------------------------------------------------------------------

    // Construct from an XML-tag

    LHAgenerator::LHAgenerator(const XMLTag & tag, std::string defname)
        : name(defname), version(defname), contents(defname) {
            for ( std::map<std::string,std::string>::const_iterator it = tag.attr.begin();
                    it != tag.attr.end(); ++it ) {
                std::string v = it->second.c_str();
                if ( it->first == "name" ) name = v;
                else if ( it->first == "version" ) version = v;
                else attributes[it->first] = v;
            }
            contents = tag.contents;
        }

    //--------------------------------------------------------------------------

    // Print out the corresponding XML-tag.

    void LHAgenerator::print(std::ostream & file) const {
        file << "<generator";
        if ( name    != "" ) file << " name=\""    << name    << "\"";
        if ( version != "" ) file << " version=\"" << version << "\"";
        for ( std::map<std::string,std::string>::const_iterator it = attributes.begin();
                it != attributes.end(); ++it )
            file << " " << it->first << "=\"" << it->second << "\"";
        file << " >";
        file << contents;
        file << "</generator>" << std::endl;
    }

    //==========================================================================

    // The LHAwgt struct: Collect the wgt information.

    //--------------------------------------------------------------------------

    // Construct from an XML-tag

    LHAwgt::LHAwgt(const XMLTag & tag, double defwgt)
        : id(""), contents(defwgt) {
            for ( std::map<std::string,std::string>::const_iterator it = tag.attr.begin();
                    it != tag.attr.end(); ++it ) {
                std::string v = it->second;
                if ( it->first == "id" ) id = v;
                else attributes[it->first] = v;
            }
            contents = atof(tag.contents.c_str());
        }

    //--------------------------------------------------------------------------

    // Print out the corresponding XML-tag.

    void LHAwgt::print(std::ostream & file) const {
        file << "<wgt";
        if ( id    != "" ) file << " id=\""    << id << "\"";
        for ( std::map<std::string,std::string>::const_iterator it = attributes.begin();
                it != attributes.end(); ++it )
            file << " " << it->first << "=\"" << it->second << "\"";
        file << " >";
        file << contents;
        file << "</wgt>" << std::endl;
    }

    //==========================================================================

    // The LHAweight struct: Collect the weight information.

    //--------------------------------------------------------------------------

    // Construct from an XML-tag.

    LHAweight::LHAweight(const XMLTag & tag, std::string defname)
        : id(defname), contents(defname) {
            for ( std::map<std::string,std::string>::const_iterator it = tag.attr.begin();
                    it != tag.attr.end(); ++it ) {
                std::string v = it->second;
                if ( it->first == "id" ) id = v;
                else attributes[it->first] = v;
            }
            contents = tag.contents;
        }

    //--------------------------------------------------------------------------

    // Print out the corresponding XML-tag.

    void LHAweight::print(std::ostream & file) const {
        file << "<weight";
        if ( id  != "" ) file << " id=\""    << id << "\"";
        for ( std::map<std::string,std::string>::const_iterator it = attributes.begin();
                it != attributes.end(); ++it )
            file << " " << it->first << "=\"" << it->second << "\"";
        file << " >";
        file << contents;
        file << "</weight>" << std::endl;
    }

    //==========================================================================

    // The LHAweightgroup struct: The LHAweightgroup assigns a group-name to a set
    // of LHAweight objects.

    //--------------------------------------------------------------------------

    // Construct a group of LHAweight objects from an XML tag and
    // insert them in the given vector.

    LHAweightgroup::LHAweightgroup(const XMLTag & tag) {

        for ( std::map<std::string,std::string>::const_iterator it = tag.attr.begin();
                it != tag.attr.end(); ++it ) {
            std::string v = it->second.c_str();
            if ( it->first == "name" ) name = v;
            else attributes[it->first] = v;
        }

        contents = tag.contents;

        // Now add the weight's step by step.
        std::string s;
        std::vector<XMLTag*> tags = XMLTag::findXMLTags(tag.contents, &s);
        for ( size_t i = 0, N = tags.size(); i < N; ++i ) {
            const XMLTag & tagnow = *tags[i];
            LHAweight wt(tagnow);
            weights.insert(make_pair(wt.id, wt));
        }
        for ( size_t i = 0, N = tag.tags.size(); i < N; ++i ) {
            const XMLTag & tagnow = *tag.tags[i];
            const LHAweight & wt(tagnow);
            weights.insert(make_pair(wt.id, wt));
        }

        for ( size_t i = 0, N = tags.size(); i < N; ++i ) if (tags[i]) delete tags[i];

    }

    //--------------------------------------------------------------------------

    // Print out the corresponding XML-tag.

    void LHAweightgroup::print(std::ostream & file) const {
        file << "<weightgroup";
        if ( name != "" ) file << " name=\"" << name << "\"";
        for ( std::map<std::string,std::string>::const_iterator it = attributes.begin();
                it != attributes.end(); ++it )
            file << " " << it->first << "=\"" << it->second << "\"";
        file << " >\n";
        for ( std::map<std::string,LHAweight>::const_iterator it = weights.begin();
                it != weights.end(); ++it ) it->second.print(file);
        file << "</weightgroup>" << std::endl;
    }

    //==========================================================================

    // The LHArwgt struct: Assigns a group-name to a set of LHAwgt objects.

    //--------------------------------------------------------------------------

    // Construct a group of LHAwgt objects from an XML tag and
    // insert them in the given vector.

    LHArwgt::LHArwgt(const XMLTag & tag) {

        for ( std::map<std::string,std::string>::const_iterator it = tag.attr.begin();
                it != tag.attr.end(); ++it ) {
            std::string v = it->second.c_str();
            attributes[it->first] = v;
        }
        contents = tag.contents;

        // Now add the wgt's step by step.
        std::string s;
        std::vector<XMLTag*> tags = XMLTag::findXMLTags(tag.contents, &s);
        for ( size_t i = 0, N = tags.size(); i < N; ++i ) {
            const XMLTag & tagnow = *tags[i];
            LHAwgt wt(tagnow);
            wgts.insert(make_pair(wt.id, wt));
        }
        for ( size_t i = 0, N = tag.tags.size(); i < N; ++i ) {
            const XMLTag & tagnow = *tag.tags[i];
            LHAwgt wt(tagnow);
            wgts.insert(make_pair(wt.id, wt));
        }

        for ( size_t i = 0, N = tags.size(); i < N; ++i ) if (tags[i]) delete tags[i];

    }

    //--------------------------------------------------------------------------

    // Print out the corresponding XML-tag.

    void LHArwgt::print(std::ostream & file) const {
        file << "<rwgt";
        for ( std::map<std::string,std::string>::const_iterator it = attributes.begin();
                it != attributes.end(); ++it )
            file << " " << it->first << "=\"" << it->second << "\"";
        file << " >\n";
        for ( std::map<std::string,LHAwgt>::const_iterator it = wgts.begin();
                it != wgts.end(); ++it ) it->second.print(file);
        file << "</rgwt>" << std::endl;
    }

    //==========================================================================

    // The LHAinitrwgt assigns a group-name to a set of LHAweightgroup objects.

    //--------------------------------------------------------------------------

    // Construct a group of LHAweightgroup objects from an XML tag and
    // insert them in the given vector.

    LHAinitrwgt::LHAinitrwgt(const XMLTag & tag) {
        for ( std::map<std::string,std::string>::const_iterator it = tag.attr.begin();
                it != tag.attr.end(); ++it ) {
            std::string v = it->second.c_str();
            attributes[it->first] = v;
        }
        contents = tag.contents;

        // Now add the wgt's step by step.
        std::string s;
        std::vector<XMLTag*> tags = XMLTag::findXMLTags(tag.contents, &s);
        for ( size_t i = 0, N = tags.size(); i < N; ++i ) {
            const XMLTag & tagnow = *tags[i];
            if ( tagnow.name == "weightgroup" ) {
                LHAweightgroup wgroup(tagnow);
                std::string wgname = wgroup.name;
                weightgroups.insert(make_pair(wgname, wgroup));
                std::string ss;
                std::vector<XMLTag*> tags2 = XMLTag::findXMLTags(tagnow.contents, &ss);
                for ( size_t k = 0, M = tags2.size(); k < M; ++k ) {
                    const XMLTag & tagnow2 = *tags2[k];
                    if ( tagnow2.name == "weight" ) {
                        LHAweight wt(tagnow2);
                        std::string wtname = wt.id;
                        weights.insert(make_pair(wtname, wt));
                    }
                }
                for ( size_t k = 0, M = tags2.size(); k < M; ++k )
                    if (tags2[k]) delete tags2[k];
            } else if ( tagnow.name == "weight" ) {
                LHAweight wt(tagnow);
                std::string wtname = wt.id;
                weights.insert(make_pair(wtname, wt));
            }
        }

        // Now add the wgt's step by step.
        for ( size_t i = 0, N = tag.tags.size(); i < N; ++i ) {
            const XMLTag & tagnow = *tag.tags[i];
            if ( tagnow.name == "weightgroup" ) {
                LHAweightgroup wgroup(tagnow);
                std::string wgname = wgroup.name;
                weightgroups.insert(make_pair(wgname, wgroup));
                std::string ss;
                std::vector<XMLTag*> tags2 = XMLTag::findXMLTags(tagnow.contents, &ss);
                for ( size_t k = 0, M = tags2.size(); k < M; ++k ) {
                    const XMLTag & tagnow2 = *tags2[k];
                    if ( tagnow2.name == "weight" ) {
                        LHAweight wt(tagnow2);
                        std::string wtname = wt.id;
                        weights.insert(make_pair(wtname, wt));
                    }
                }
                for ( size_t k = 0, M = tagnow.tags.size(); k < M; ++k ) {
                    const XMLTag & tagnow2 = *tagnow.tags[k];
                    if ( tagnow2.name == "weight" ) {
                        LHAweight wt(tagnow2);
                        std::string wtname = wt.id;
                        weights.insert(make_pair(wtname, wt));
                    }
                }
                for ( size_t k = 0, M = tags2.size(); k < M; ++k )
                    if (tags2[k]) delete tags2[k];
            } else if ( tagnow.name == "weight" ) {
                LHAweight wt(tagnow);
                std::string wtname = wt.id;
                weights.insert(make_pair(wtname, wt));
            }
        }

        for ( size_t i = 0, N = tags.size(); i < N; ++i ) if (tags[i]) delete tags[i];

    }

    //--------------------------------------------------------------------------

    // Print out the corresponding XML-tag.

    void LHAinitrwgt::print(std::ostream & file) const {
        file << "<initrwgt";
        for ( std::map<std::string,std::string>::const_iterator it = attributes.begin();
                it != attributes.end(); ++it )
            file << " " << it->first << "=\"" << it->second << "\"";
        file << " >\n";
        for ( std::map<std::string,LHAweightgroup>::const_iterator it = weightgroups.begin();
                it != weightgroups.end(); ++it ) it->second.print(file);
        for ( std::map<std::string,LHAweight>::const_iterator it = weights.begin();
                it != weights.end(); ++it ) it->second.print(file);
        file << "</initrgwt>" << std::endl;
    }

    //==========================================================================

    // The HEPEUP class is a simple container corresponding to the Les Houches
    // accord (<A HREF="http://arxiv.org/abs/hep-ph/0109068">hep-ph/0109068</A>)
    // common block with the same name. The members are named in the same
    // way as in the common block. However, fortran arrays are represented
    // by vectors, except for the arrays of length two which are
    // represented by pair objects.

    //--------------------------------------------------------------------------

    // Copy information from the given HEPEUP.

    HEPEUP & HEPEUP::setEvent(const HEPEUP & x) {

        NUP = x.NUP;
        IDPRUP = x.IDPRUP;
        XWGTUP = x.XWGTUP;
        XPDWUP = x.XPDWUP;
        SCALUP = x.SCALUP;
        AQEDUP = x.AQEDUP;
        AQCDUP = x.AQCDUP;
        IDUP = x.IDUP;
        ISTUP = x.ISTUP;
        MOTHUP = x.MOTHUP;
        ICOLUP = x.ICOLUP;
        PUP = x.PUP;
        VTIMUP = x.VTIMUP;
        SPINUP = x.SPINUP;
        heprup = x.heprup;
        scales = x.scales;
        weights = x.weights;
        weights_detailed = x.weights_detailed;
        weights_compressed = x.weights_compressed;
        rwgt = x.rwgt;
        attributes = x.attributes;
        return *this;

    }

    //--------------------------------------------------------------------------

    // Reset the HEPEUP object.

    void HEPEUP::reset() {
        NUP = 0;
        weights_detailed.clear();
        weights_compressed.clear();
        weights.clear();
        rwgt.clear();
        scales.clear();
        attributes.clear();
    }

    //--------------------------------------------------------------------------

    // Assuming the NUP variable, corresponding to the number of
    // particles in the current event, is correctly set, resize the
    // relevant vectors accordingly.

    void HEPEUP::resize() {
        IDUP.resize(NUP);
        ISTUP.resize(NUP);
        MOTHUP.resize(NUP);
        ICOLUP.resize(NUP);
        PUP.resize(NUP, std::vector<double>(5));
        VTIMUP.resize(NUP);
        SPINUP.resize(NUP);
    }

    //==========================================================================

    // The Reader class is initialized with a stream from which to read a
    // version 1/2 Les Houches Accord event file. In the constructor of
    // the Reader object the optional header information is read and then
    // the mandatory init is read. After this the whole header block
    // including the enclosing lines with tags are available in the public
    // headerBlock member variable. Also the information from the init
    // block is available in the heprup member variable and any additional
    // comment lines are available in initComments. After each successful
    // call to the readEvent() function the standard Les Houches Accord
    // information about the event is available in the hepeup member
    // variable and any additional comments in the eventComments
    // variable. A typical reading sequence would look as follows:

    //--------------------------------------------------------------------------

    // Used internally in the constructors to read header and init blocks.
    bool Reader::init() {

        bool readingHeader = false;
        bool readingInit = false;

        // Make sure we are reading a LHEF file:
        getLine();

        if ( currentLine.find("<LesHouchesEvents" ) == std::string::npos )
            return false;
        version = 0;
        if ( currentLine.find("version=\"1" ) != std::string::npos )
            version = 1;
        else if ( currentLine.find("version=\"2" ) != std::string::npos )
            version = 2;
        else if ( currentLine.find("version=\"3" ) != std::string::npos )
            version = 3;
        else
            return false;

        // Loop over all lines until we hit the </init> tag.
        while ( getLine() && currentLine.find("</init>") == std::string::npos ) {
            if ( currentLine.find("<header") != std::string::npos ) {
                // We have hit the header block, so we should dump this and
                // all following lines to headerBlock until we hit the end of
                // it.
                readingHeader = true;
                headerBlock = currentLine + "\n";
            }
            else if ( currentLine.find("<init>") != std::string::npos
                    || currentLine.find("<init ") != std::string::npos ) {
                // We have hit the init block, so we should expect to find the
                // standard information in the following.
                readingInit = true;

                // The first line tells us how many lines to read next.
                getLine();
                std::istringstream iss(currentLine);
                if ( !( iss >> heprup.IDBMUP.first >> heprup.IDBMUP.second
                            >> heprup.EBMUP.first >> heprup.EBMUP.second
                            >> heprup.PDFGUP.first >> heprup.PDFGUP.second
                            >> heprup.PDFSUP.first >> heprup.PDFSUP.second
                            >> heprup.IDWTUP >> heprup.NPRUP ) ) {
                    heprup.NPRUP = static_cast<size_t>(-1);
                    return false;
                }
                heprup.resize();

                for ( size_t i = 0; i < heprup.NPRUP; ++i ) {
                    getLine();
                    std::istringstream isss(currentLine);
                    if ( !( isss >> heprup.XSECUP[i] >> heprup.XERRUP[i]
                                >> heprup.XMAXUP[i] >> heprup.LPRUP[i] ) ) {
                        heprup.NPRUP = static_cast<size_t>(-1);
                        return false;
                    }
                }
            }
            else if ( currentLine.find("</header>") != std::string::npos ) {
                // The end of the header block. Dump this line as well to the
                // headerBlock and we're done.
                readingHeader = false;
                headerBlock += currentLine + "\n";
            }
            else if ( readingHeader ) {
                // We are in the process of reading the header block. Dump the
                // line to headerBlock.
                headerBlock += currentLine + "\n";
                headerComments += currentLine + "\n";
            }
            else if ( readingInit ) {
                // Here we found a comment line. Dump it to initComments.
                initComments += currentLine + "\n";
            }
            else {
                // We found some other stuff outside the standard tags.
                outsideBlock += currentLine + "\n";
            }
        }

        if ( !file ) heprup.NPRUP = static_cast<size_t>(-1);

        // Scan the header block for XML tags
        std::string leftovers;
        std::vector<XMLTag*> tags1 = XMLTag::findXMLTags(headerComments, &leftovers);
        if ( leftovers.find_first_not_of(" \t\n") == std::string::npos )
            leftovers="";

        for ( size_t i = 0, N = tags1.size(); i < N; ++i ) {
            const XMLTag & tag = *tags1[i];

            if ( tag.name == "initrwgt" ) {
                LHAinitrwgt irwgt(tag);
                heprup.initrwgt = irwgt;
                for ( size_t j = 0, M = tag.tags.size(); j < M; ++j ) {
                    XMLTag & ctag = *tag.tags[j];
                    if ( ctag.name == "weightgroup" ) {
                        LHAweightgroup wgroup(ctag);
                        std::string wgname = wgroup.name;
                        heprup.weightgroups.insert(make_pair(wgname, wgroup));

                        std::string ss;
                        std::vector<XMLTag*> tags2 = XMLTag::findXMLTags(ctag.contents, &ss);
                        for ( size_t k = 0, O = tags2.size(); k < O; ++k ) {
                            const XMLTag & tagnow2 = *tags2[k];
                            if ( tagnow2.name == "weight" ) {
                                LHAweight wt(tagnow2);
                                std::string wtname = wt.id;
                                heprup.weights.insert(make_pair(wtname, wt));
                            }
                        }
                        for ( size_t k = 0, O = ctag.tags.size(); k < O; ++k ) {
                            const XMLTag & tagnow2 = *ctag.tags[k];
                            if ( tagnow2.name == "weight" ) {
                                LHAweight wt(tagnow2);
                                std::string wtname = wt.id;
                                heprup.weights.insert(make_pair(wtname, wt));
                            }
                        }
                    } else if ( ctag.name == "weight" ) {
                        std::string tname = ctag.attr["id"];
                        heprup.weights.insert(make_pair(tname, LHAweight(ctag)));
                    }
                }
            }
        }

        heprup.generators.clear();
        // Scan the init block for XML tags
        leftovers="";
        std::vector<XMLTag*> tags2 = XMLTag::findXMLTags(initComments, &leftovers);
        if ( leftovers.find_first_not_of(" \t\n") == std::string::npos )
            leftovers="";

        for ( size_t i = 0, N = tags2.size(); i < N; ++i ) {
            const XMLTag & tag = *tags2[i];
            if ( tag.name == "generator" ) {
                heprup.generators.push_back(LHAgenerator(tag));
            }
        }

        for ( size_t i = 0, N = tags1.size(); i < N; ++i ) if (tags1[i]) delete tags1[i];
        for ( size_t i = 0, N = tags2.size(); i < N; ++i ) if (tags2[i]) delete tags2[i];

        // Done
        return true;

    }

    //--------------------------------------------------------------------------

    // Read an event from the file and store it in the hepeup
    // object. Optional comment lines are stored in the eventComments
    // member variable. return true if the read was successful.

    bool Reader::readEvent(HEPEUP * peup) {

        HEPEUP & eup = (peup? *peup: hepeup);
        eup.clear();
        eup.heprup = &heprup;

        // Check if the initialization was successful. Otherwise we will
        // not read any events.
        if ( heprup.NPRUP != static_cast<size_t>(-1) ) return false;
        eventComments = "";
        outsideBlock = "";
        eup.NUP = 0;

        // Keep reading lines until we hit the next event or the end of
        // the event block. Save any inbetween lines. Exit if we didn't
        // find an event.
        while ( getLine() && currentLine.find("<event") == std::string::npos )
            outsideBlock += currentLine + "\n";

        // Get event attributes.
        if (currentLine != "") {
            std::string eventLine(currentLine);
            eventLine += "</event>";
            std::vector<XMLTag*> evtags = XMLTag::findXMLTags(eventLine);
            XMLTag & evtag = *evtags[0];
            for ( std::map<std::string,std::string>::const_iterator it = evtag.attr.begin();
                    it != evtag.attr.end(); ++it ) {
                std::string v = it->second.c_str();
                eup.attributes[it->first] = v;
            }
            for ( size_t i = 0, N = evtags.size(); i < N; ++i ) if (evtags[i]) delete evtags[i];
        }

        if ( !getLine()  ) return false;

        // We found an event. The first line determines how many
        // subsequent particle lines we have.
        std::istringstream iss(currentLine);
        if ( !( iss >> eup.NUP >> eup.IDPRUP >> eup.XWGTUP
                    >> eup.SCALUP >> eup.AQEDUP >> eup.AQCDUP ) )
            return false;
        eup.resize();

        // Read all particle lines.
        for ( size_t i = 0; i < eup.NUP; ++i ) {
            if ( !getLine() ) return false;
            std::istringstream isss(currentLine);
            if ( !( isss >> eup.IDUP[i] >> eup.ISTUP[i]
                        >> eup.MOTHUP[i].first >> eup.MOTHUP[i].second
                        >> eup.ICOLUP[i].first >> eup.ICOLUP[i].second
                        >> eup.PUP[i][0] >> eup.PUP[i][1] >> eup.PUP[i][2]
                        >> eup.PUP[i][3] >> eup.PUP[i][4]
                        >> eup.VTIMUP[i] >> eup.SPINUP[i] ) )
                return false;
        }

        // Now read any additional comments.
        while ( getLine() && currentLine.find("</event>") == std::string::npos )
            eventComments += currentLine + "\n";

        if ( !file ) return false;

        eup.scales = LHAscales(eup.SCALUP);

        // Scan the init block for XML tags
        std::string leftovers;
        std::vector<XMLTag*> tags = XMLTag::findXMLTags(eventComments, &leftovers);
        if ( leftovers.find_first_not_of(" \t\n") == std::string::npos )
            leftovers="";

        eventComments = "";
        std::istringstream f(leftovers);
        std::string l;    
        while (std::getline(f, l)) {
            size_t p = l.find_first_not_of(" \t");
            l.erase(0, p);
            p = l.find_last_not_of(" \t");
            if (std::string::npos != p) l.erase(p+1);
            if (l.find_last_not_of("\n") != std::string::npos)
                eventComments += l + "\n";
        }

        for ( size_t i = 0, N = tags.size(); i < N; ++i ) {
            XMLTag & tag = *tags[i];

            if ( tag.name == "weights" ) {
                LHAweights wts(tag);
                eup.weights = wts;

                for ( size_t k = 0, M = wts.weights.size(); k < M; ++k ) {
                    eup.weights_compressed.push_back(wts.weights[k]);
                }

            }
            else if ( tag.name == "scales" ) {
                eup.scales = LHAscales(tag, eup.SCALUP);
            }
            else if ( tag.name == "rwgt" ) {
                LHArwgt rwgt(tag);
                eup.rwgt = rwgt;
                std::string s;
                std::vector<XMLTag*> tags2 = XMLTag::findXMLTags(rwgt.contents, &s);
                for ( size_t k = 0, M = tags2.size(); k < M; ++k ) {
                    const XMLTag & tagnow = *tags2[k];
                    if ( tagnow.name == "wgt" ) {
                        LHAwgt wt(tagnow);
                        eup.weights_detailed.insert(make_pair(wt.id, wt.contents));
                    }
                }
                for ( size_t k = 0, M = tag.tags.size(); k < M; ++k ) {
                    const XMLTag & tagnow = *tag.tags[k];
                    if ( tagnow.name == "wgt" ) {
                        LHAwgt wt(tagnow);
                        eup.weights_detailed.insert(make_pair(wt.id, wt.contents));
                    }
                }
            }
        }

        for ( size_t i = 0, N = tags.size(); i < N; ++i ) if (tags[i]) delete tags[i];

        return true;

    }

    //==========================================================================

    // The Writer class is initialized with a stream to which to write a
    // version 3.0 Les Houches Accord event file.

    //--------------------------------------------------------------------------

    // Write out an optional header block followed by the standard init
    // block information together with any comment lines.

    void Writer::init() {

        // Write out the standard XML tag for the event file.
        if ( version == 1 )
            file << "<LesHouchesEvents version=\"1.0\">" << std::endl;
        else
            file << "<LesHouchesEvents version=\"3.0\">" << std::endl;

        file << std::setprecision(8);

        // Print headercomments and header init information.
        file << "<header>" << std::endl;
        file << hashline(headerStream.str(),true) << std::flush; 
        if ( version != 1 ) heprup.initrwgt.print(file);
        file << "</header>" << std::endl;

        file << "<init>"<< std::endl 
            << " " << std::setw(8) << heprup.IDBMUP.first
            << " " << std::setw(8) << heprup.IDBMUP.second
            << " " << std::setw(14) << heprup.EBMUP.first
            << " " << std::setw(14) << heprup.EBMUP.second
            << " " << std::setw(4) << heprup.PDFGUP.first
            << " " << std::setw(4) << heprup.PDFGUP.second
            << " " << std::setw(4) << heprup.PDFSUP.first
            << " " << std::setw(4) << heprup.PDFSUP.second
            << " " << std::setw(4) << heprup.IDWTUP
            << " " << std::setw(4) << heprup.NPRUP << std::endl;
        heprup.resize();
        for ( size_t i = 0; i < heprup.NPRUP; ++i )
            file << " " << std::setw(14) << heprup.XSECUP[i]
                << " " << std::setw(14) << heprup.XERRUP[i]
                << " " << std::setw(14) << heprup.XMAXUP[i]
                << " " << std::setw(6) << heprup.LPRUP[i] << std::endl;

        if ( version == 1 ) {
            file << hashline(initStream.str(),true) << std::flush
                << "</init>" << std::endl;
            initStream.str("");
            return;
        }

        for ( size_t i = 0, N = heprup.generators.size(); i < N; ++i ) {
            heprup.generators[i].print(file);
        }

        file << hashline(initStream.str(),true) << std::flush
            << "</init>" << std::endl;
        initStream.str("");
    }

    //--------------------------------------------------------------------------

    // Write out the event stored in hepeup, followed by optional
    // comment lines.

    bool Writer::writeEvent(HEPEUP * peup) {

        HEPEUP & eup = (peup? *peup: hepeup);

        file << "<event";
        for ( std::map<std::string,std::string>::const_iterator it = eup.attributes.begin();
                it != eup.attributes.end(); ++it )
            file << " " << it->first << "=\"" << it->second << "\"";
        file << ">" << std::endl;
        file << " " << std::setw(4) << eup.NUP
            << " " << std::setw(6) << eup.IDPRUP
            << " " << std::setw(14) << eup.XWGTUP
            << " " << std::setw(14) << eup.SCALUP
            << " " << std::setw(14) << eup.AQEDUP
            << " " << std::setw(14) << eup.AQCDUP << std::endl;
        eup.resize();

        for ( size_t i = 0; i < eup.NUP; ++i )
            file << " " << std::setw(8) << eup.IDUP[i]
                << " " << std::setw(2) << eup.ISTUP[i]
                << " " << std::setw(4) << eup.MOTHUP[i].first
                << " " << std::setw(4) << eup.MOTHUP[i].second
                << " " << std::setw(4) << eup.ICOLUP[i].first
                << " " << std::setw(4) << eup.ICOLUP[i].second
                << " " << std::setw(18) << eup.PUP[i][0]
                << " " << std::setw(18) << eup.PUP[i][1]
                << " " << std::setw(18) << eup.PUP[i][2]
                << " " << std::setw(18) << eup.PUP[i][3]
                << " " << std::setw(18) << eup.PUP[i][4]
                << " " << std::setw(1) << eup.VTIMUP[i]
                << " " << std::setw(1) << eup.SPINUP[i] << std::endl;

        // Write event comments.
        file << hashline(eventStream.str()) << std::flush;
        eventStream.str("");

        if ( version != 1 ) {
            eup.rwgt.print(file);
            eup.weights.print(file);
            eup.scales.print(file);
        }

        file << "</event>" << std::endl;

        if ( !file ) return false;

        return true;

    }

    //--------------------------------------------------------------------------

    // Make sure that each line in the string \a s starts with a
    // #-character and that the string ends with a new-line.

    std::string Writer::hashline(std::string s, bool comment) {
        std::string ret;
        std::istringstream is(s);
        std::string ss;
        while ( getline(is, ss) ) {
            if ( comment )
                ss = "# " + ss;
            ret += ss + '\n';
        }
        return ret;
    }

    //==========================================================================

}
