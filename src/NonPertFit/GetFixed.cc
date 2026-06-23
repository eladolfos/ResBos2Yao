#include "NonPertFit/FixedTarget.hh"
#include "ResBos/loguru.hpp"

int main(int argc, char** argv)
{
    if(argc != 2){
      std::cout<<"usage: GetFixed datafile"<<std::endl;
      return 0;
    }

    std::string fitting_config = "fixedtarget.config";
    // Startup logger
    loguru::g_stderr_verbosity = loguru::Verbosity_WARNING;

    FixedTarget m;
    m.global_settings = std::unique_ptr<IO::Settings>(new IO::Settings(fitting_config));
    m.ReadFile(argv[1]);

    return 1;
}
