%module pyResbos

%{
    #define SWIG_FILE_WITH_INIT
    #include "ResBos/PyResBos.hh"
    using namespace ResBos;
%}

%include "std_map.i"
%include "std_string.i"
%include "std_vector.i"
%include "numpy.i"
%include "argcargv.i"
%apply (int ARGC, char **ARGV) { (int argc, char *argv[]) }

%template(vectord) std::vector<double>;

%init %{
import_array();
%}

%apply (int DIM1, int DIM2, double* IN_ARRAY2) {(int nevents, int vars, double* array)};

%include "ResBos/PyResBos.hh"
