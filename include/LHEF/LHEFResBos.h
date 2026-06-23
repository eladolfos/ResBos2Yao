#ifndef LHAUP_H
#define LHAUP_H

#include <memory>

/*********************************************************************
 * A set of classes to implement the LHEF3 file format.
 * Based off of the implementation found in Pythia8
 ********************************************************************/

#include "LHEF/LHEF3.h"

class Event;

class LHEF3FromResBos {
    public:
        LHEF3FromResBos(std::shared_ptr<Event> event_) : writer(osLHEF), event(event_) {}
        virtual ~LHEF3FromResBos() {}

        virtual bool OpenLHEF(std::string);
        virtual bool SetInit();
        virtual bool SetEvent(const Event&);
        virtual bool CloseLHEF(bool updateInit = false);

    private:
        // LHEF3 writer
        LHEF::Writer writer;
        std::string filename;
        std::fstream osLHEF;

        // Number of digits to set width of double write out
        int pDigits;
        bool writeToFile;

        // Some internal init and event block objects for convenience
        LHEF::HEPRUP heprup;
        LHEF::HEPEUP hepeup;

        std::shared_ptr<Event> event;
};


#endif
