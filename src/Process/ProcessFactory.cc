#include "ResBos/Process.hh"

#include <iostream>

#include "ResBos/loguru.hpp"

namespace ResBos {

ProcessFactory& ProcessFactory::Instance() {
    static ProcessFactory factory = ProcessFactory();
    return factory;
}

bool ProcessFactory::Register(const std::string& name, 
                              ProcessFactory::TCreateMethod funcCreate) {
    auto it = methods.find(name);
    if(it == methods.end()) {
        methods[name] = funcCreate;
        return true;
    }
    return false;
}

std::unique_ptr<Process> ProcessFactory::Create(const std::string& name, std::shared_ptr<ResBos> r, const IO::Settings &s) {
    auto it = methods.find(name);
    if(it != methods.end())
        return it->second(r,s);

    return nullptr;
}

}
