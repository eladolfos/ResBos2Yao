#include "ResBos/Calculation.hh"

#include <iostream>

#include "ResBos/loguru.hpp"

namespace ResBos {

CalculationFactory& CalculationFactory::Instance() {
    static CalculationFactory factory = CalculationFactory();
    return factory;
}

bool CalculationFactory::Register(const std::string name,
                                  CalculationFactory::TCreateMethod funcCreate) {
    auto it = methods.find(name);
    if(it == methods.end()) {
        methods.insert(std::make_pair(name,funcCreate));
//        LOG_F(INFO,"Registered Calculation method: %s", name);
        return true;
    }
    return false;
}

std::unique_ptr<Calculation> CalculationFactory::Create(const std::string& name) {
    auto it = methods.find(name);
    if(it != methods.end())
        return it -> second();

    return nullptr;
}

}
