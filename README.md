#  ResBos2
## Version 2.0.0 
[Download](https://gitlab.msu.edu/resummation/ResBos2/repository/archive.zip) Current Version

### Table of Contents
1. [Description](#description)
2. [Installation](#installation)
  1. [Requirements](#requirements)
  2. [How To Install](#how-to-install)
3. [Usage](#usage)
4. [Particle Processes](#particle-processes)
5. [References](#references)
6. [Bug Reporting](#bug-reporting)
7. [Contact Us](#contact-us)

### Description 
ResBos is a monte-carlo program to obtain fully differential cross-sections for
different processes including the effects of resummation. A list of all available
processes can be found below and the order in which the process is calculated to
is noted after each process.

### Installation 
First you need to download most recent version of the code from the link above.

#### Requirements 
- C++11: Used for the functionality that was added, and to not require BOOST libraries
- LHAPDF: Used for allowing the user to use any desired PDF
- HOPPET: Used for preforming convolutions with splitting functions for asymptotic calculations

### Optional Packages
- ROOT: For histogramming
- YODA: For histogramming
- GZIP: For zipping up grids to reduce storage size
- MPI: To allow for multiprocessing of the code

### CMake Flags
| Option Name            | Description                                           |
|------------------------|-------------------------------------------------------|
| CMAKE\_INSTALL\_PREFIX | Install location                                      |
| Hoppet\_ROOT\_DIR      | Location of Hoppet                                    |
| LHAPDF\_ROOT\_DIR      | Location of LHAPDF                                    |
| YODA\_ROOT\_DIR        | Location of YODA                                      |
| USE-ROOT               | Turn on Root Histogramming                            |
| USE-YODA               | Turn on YODA Histogramming                            |
| USE-GZIP               | Turn on reading/writing gzipped files                 |
| USE-MPI                | Turn on MPI for grid generation and VEGAS integration |
| ZLIB\_INCLUDE\_DIR     | Location of ZLIB                                      |

### How To Install
1. mkdir build
2. cd build
3. cmake .. [-DCMAKE\_OPTIONS]
4. make [-jN]
5. make [-jN] install

### Usage 
The code can be run with the following command:

$ /path/to/code/resbos

Additionally, specific options can be set on the command line that will overwrite the default values. These options are:
- -i,--input: Change the input file to be used for the ResBos2 run parameters. The default value is resbos.config.
- -o,--output: Output folder to store the output histograms and files from the running of the code. The default value is the current working directory.
- -s,--seed: The seed value for the random number generator. Default is read from the input file. 
- --itmax1,--itmax2: Number of iterations for the VEGAS warm-up/final run.
- --ncall1,--ncall2: Number of event calls per iteration for the warm-up/final run of VEGAS.
- -w,--write: Output the events generated in LHE format. Takes either true or false as input.
- -g,--gridGen: If the flag is true, generate grids for the interpolation to speed up the calculation. Otherwise, preforms _b_-space integral for each phase space point.
- -v: Change the verbosity of the logging. Takes an integer from 0-10, with 0 being less verbose, and 10 being extremely verbose.

### Input file options

#### Running Mode Info
- mode: Either MCFull (full phase space integration) or GridGen (generate resummed grid in Q,pT and y).
- XSec: Options include:
    - Resummation: Resummed calculation only
    - Asymptotic: Asymptotic expansion only
    - DeltaSigma: Integrate expansion to fixed pT
    - FixedOrder: Leading order fixed order calculation
    - YTerm: Difference between fixed order and asymptotic
    - WmA: Difference between resummed and asymptotic, used for MCFM interface
    - Total: Resummed plus the y term

#### VEGAS PARAMETERS
- itmax1/itmax2: number of iterations for VEGAS warm-up/final run
- ncall1/ncall2: number of function calls per iteration in warm-up/final run
- iSeed: random seed for reproducibility, -1 will choose a random seed

#### Process Info
- beam: the type of beam in the collision
- ECM: center of mass energy of the collision
- Process: The current options are:
    - DrellYan
    - ZU: Drell-Yan with only up-type quarks
    - ZD: Drell-Yan with only down-type quarks
    - Z0: Only the Z boson, no photon
    - A0: Only the photon, no Z boson
    - WPlus
    - WMinus
    - Higgs
- CKM: Whether W boson interactions should use the CKM matrix or not
- HiggsDecay: Decay mode of the higgs. Current options include:
    - None
    - AA
    - ZZ
    - WW
- PDF: The LHAPDF pdf name
- iSet: The set number of the PDF to use
- KinematicCorrection: Whether to use the traditional x1, x2 definitions, or include a pT correction factor

#### Resummation Parameters
- scheme: CSS / CFG
- AOrder: Order of the A coefficient
- BOrder: Order of the B coefficient
- COrder: Order of the C coefficient
- HOrder: Order of the H coefficient
- C1,C2,C3: Scaling factor for C1,C2,C3
- NonPert: Non-perturbative functional forms. Options are:
    - BLNY
    - SIYY
- bMax: Maximum b in the b\* prescription
- ngag: Not used currently
- g: A list of the values for the non-pert parameters, must be put in as a comma separated list

#### Fixed-Order/Asymptotic Parameter
- PertOrder: Order for the perturbative calculation (NOTE: only 1 is implemented currently)
- AsymOrder: Asymptotic expansion order
- DeltaSigmaOrder: Order for the DeltaSigma calculation
- scale: Choice for the central scale of the calculation. Choices are Q or MT.
- muF, muR: Scaling factor for muF, muR

#### EW Inputs
- EWMassScheme: OnShell is currently the only one implemented and fully tested
- mT, mH, mW, mZ: Masses for the top, Higgs, W, and Z boson
- gamT, gamH, gamW, gamZ: Widths for the top, Higgs, W, and Z boson
- mu, md, ms, mc, mb: Inpupt masses for remaining quarks
- EWInputs: How to define remaining EW parameters from 3 inputs. Currently just GFMWMZ fully implemented and tested
- GF, AlphaEM0. AlphaEMMZ, sinw2: Input for GF, alpha\_em at 0 and MZ, and sin^2(theta\_W)
- AlphaEMOrder: Order for the running of alpha\_em

#### Cuts:
For all of the following (with the exception of qMax, qtMax, and all rapidities), a choice of -1 means no maximum value.
- qMin, qMax: Minimum and maximum invariant mass range to generate
- qtMin, qtMax: Minimum and maximum transverse momentum to generate
- yMin, yMax: Minimum and maximum rapidity to generate
- ptlMin, ptlMax: Minimum and maximum transverse momentum for leptons
- ylMin, ylMax: Minimum and maximum rapidity for leptons
- METMin, METMax: Minimum and maximum missing transverse energy
- drllMin, drllMax: Minimum and maximum R separation between leptons
- ptl1Min, ptl2Min, ptl1Max, ptl2Max: Minimum and maximum cuts for leptons 1 and 2. This allows for asymmetric pt cuts.

#### Histograms
- HistogramMode: How the histograms should be saved. Options are:
    - BuiltIn: Just basic histogramming to a text file
    - YODA: Use YODA to write histograms (Need to enable with -DUSE-YODA in the cmake command)
    - ROOT: Use ROOT to write histograms (Need to enable with -DUSE-ROOT in the cmake command)
- WriteEvent: Whether to write out the intermediate bosons and final state particles to a LHEF file format for processing with other tools.

### Particle Processes

**_NOTE_**: Not all of the following are currently implemented, but will be in the first release

The following table summarizes all the particle processes that included inside the code for the given version, along with
the references to the papers related to the process.

Process | Fixed Order | Resummed Order | References
--------|-------------|----------------|-----------
W/Z/A   | NNLO        | NNLL           | 1-4,9,12,13
Higgs   | NNLO        | NNLL           | 9,10,19
Higgs MSSM | NLO      | NNLL           | 8,14,15
AA      | NLO         | NNLL           | 5,7,11,16-18
ZZ      | NLO         | NLL            | 6
H+Jet   | NLO         | NLL            | 

### References
1. [The nonperturbative regime in QCD resummation for guage boson production at hadron colliders]()
2. [Effects of QCD resummation on distributions of leptons from the decay of electroweak vector bosons]()
3. [Testing multiple gluon dynamics at the Tevatron]()
4. [Soft gluon effects on lepton pairs at hadron colliders]()
5. [Photon pair production with soft gluon resummation in hadronic interactions]()
6. [Higgs boson production at hadron colliders with soft gluon effects. 1. Backgrounds]()
7. [Diphoton background to Higgs boson production at the LHC with soft gluon effects]() 
8. [QCD corrections to scalar production via heavy quark fusion at hadron colliders]()
9. [Higgs production: A comparison of parton showers and resummation]()
10. [Higgs boson production at the LHC with soft gluon effects]()
11. [Diphoton production in gluon fusion at small transverse momentum]()
12. [Tevatron Run-1 Z boson data and Collins-Soper-Sterman resummation formalism]()
13. [Universality of the Collins-Soper-Sterman nonperturbative function in gauge boson production]()
14. [Heavy-flavor effects in soft gluon resummation for electroweak boson production at hadron colliders]() 
15. [Transverse momentum resummation for Higgs boson produced via b anti-b fusion at hadron colliders]()
16. [All-orders resummation for diphoton production at hadron colliders]()
17. [Calculation of prompt diphton production cross-sections at Tevatron and LHC energies]()
18. [Gluon-gluon contributions to the production of continuum diphoton pairs at hadron colliders]()
19. [Resummation Effects in the Search of SM Higgs Boson at Hadron Colliders]()

### Bug Reporting

To report a bug that you have found, please send the resbos.config file and the resbos.log file to isaacson@fnal.gov. In addition, please give a description of what you expected to happen, what did happen, and what (if any) have you tried to address the problem.

### Contact Us

For questions about the code, please contact isaacson@fnal.gov
