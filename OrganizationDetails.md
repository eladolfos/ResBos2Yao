# ResBos2 Code Organization Document

## Main Classes
1. Process (Final state and hard parts)
2. Calculation (Resummation, Asymptotic, and Singular Pieces)
3. Collider / Beam information
4. Utilities / Tools / Information
5. User Interface

## Subclasses

| Subclass        | Depends On                          | Used Mainly in | Notes                                                                                                       |
|-----------------|-------------------------------------|----------------|-------------------------------------------------------------------------------------------------------------|
| PDF             | Collider/Beam                       | Calculation    | Implemented by LHAPDF, should return values based on the beam type and ecm                                  |
| Hoppet          | Collider/Beam, PDF                  | Calculation    | Hoppet is used to preform convolutions of the PDF with the splitting funcitons                              |
| Convolution     | Collider/Beam, Process, PDF, Hoppet | Calculation    | Calculates and stores information about the convolution of the PDF with splitting functions and C funcitons |
| Beam            | PDF, Settings                       | Calculation    | Stores information about the beam type and center of mass energy                                            |
| ConfigParser    | Nothing                             | Utilities      | Used to parse the input files and extract the settings                                                      |
| Crossing        | Nothing                             | Calculation    | Used to find the crossing point of the Resummed and Asymptotic piece                                        |
| Electroweak     | Nothing                             | Process        | Stores information about the electroweak parameters used in calculating the hard matrix elements            |
| Enums           | Nothing                             | Everything     | Just a file to store information about the enums used throughout the code for bookkeeping purposes          |
| Event           | FourVector, Particle, LHEF          | User Interface | Used to store the kinematics for the event to write to both histograms and to an LHE file for external uses |
| Fit             | Everything                          | BAT Interface  | Used to improve the speed of the calculation for fitting the non-perturbative function                      |
| FourVector      | Nothing                             | Utilities      | Used to store four momentum information of particles in                                                     |
| Grid2D          | Nothing                             | Calculation    | Stores two dimensional grid information that can be interpolated to speed up the calculation                |
| Grid3D          | Nothing                             | Calculation    | Stores three dimensional grid information that can be interpolated to speed up the calculation              |
| GridIO          | Grid2D, Grid3D                      | User Interface | Loads or saves both 2D and 3D grids to prevent the need to calculate the grids more than once               |
| Histogram       | ROOT/YODA/Nothing                   | User Interface | Outputs histogram information in the form of a root/yoda file, or as a text file                            |
| HoppetInterface | Nothing                             | Calculation    | Provides an interface between the Fortran Hoppet code and the C++ ResBos2 code                              |
| LHEF            | Nothing                             | User Interface | Outputs events in the lhe format                                                                            |
| Logger          | Nothing                             | Internal       | Used to log information about what is occuring for debugging and user information purposes                  |
| Log             | Nothing                             | Internal       | Setups the logging class that is used by Logger                                                             |
| Particle        | Nothing                             | Process        | Creates a particle that can be used by the Event and Process class                                          |
| PhaseSpace      | Nothing                             | Process        | Generates a phase space point for the given process according to the number of variables in the hard xsec   |
| Process         | Electroweak, PhaseSpace, Settings   | Process        | Preforms process specfic calculations, such as the hard martix elements                                     |
| QCDConst        | Nothing                             | Calculation    | Contains constants that define QCD parameters                                                               |
| Random          | Nothing                             | Utilities      | Generates random numbers to be used in the VEGAS integration                                                |
| ResBos2         | Process,                            | Calculation    | This is the main interface to contain things that are used by all calculation types                         |
| Settings        | ConfigParser                        | Internal       | Stores information read from the user input cards to setup the needed pieces of the calculation             |
| ThreadPool      | Nothing                             | Utilities      | Creates, maintains, and destorys a basic thread pool to allow for basic parallel calculations               |
| ThreeVector     | Nothing                             | Utilities      | Stores three vector information                                                                             |
| Utilitiy        | Nothing                             | Utilities      | Basic integration routines and other utility functions                                                      |
| Vegas           | Nothing                             | Utilities      | Preforms the vegas integration over the allowed phase space to generate events and calculate the total xsec |

## Organization of class structures:

### Classes containing no other class variables:
* PDF
* Hoppet
* Convolution
* ConfigParser
* Electroweak
* FourVector
* ThreeVector
* Grid2D
* Grid3D
* GridIO
* Histogram
* LHEF
* Logger
* Log
* Particle
* QCDConst
* Random
* ThreadPool
* Utility
