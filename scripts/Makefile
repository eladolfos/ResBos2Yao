PROGRAM=wpAnalysis

OBJECTS=WprimeAnalysis.o

CXX=g++
FORT=gfortran

CXXFLAGS=-O2 -std=c++11 -Wall -fopenmp
LDFLAGS=-lstdc++ -lgfortran -lm

ROOTCFLAGS=$(shell root-config --cflags)
ROOTLIBS=$(shell root-config --libs)

$(PROGRAM):$(OBJECTS)
	$(CXX) -o $(PROGRAM) $(OBJECTS) $(LDFLAGS) $(ROOTLIBS)

TopDecay:topDecay.o ../src/utility.o ../src/vegas.o
	$(CXX) -o TopDecay topDecay.o ../src/utility.o ../src/vegas.o $(LDFLAGS) $(ROOTLIBS)

MergeMCFMResBos: MergeMCFMResBos.o
	$(CXX) -o MergeMCFMResBos MergeMCFMResBos.o $(LDFLAGS) $(ROOTLIBS)

%.o: %.cc
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) -c -o $@ $<

%.o: %.f
	$(FORT) -c -o $@ $<
