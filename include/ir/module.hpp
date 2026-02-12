#pragma once

#include "type.hpp"
#include "value.hpp"
#include "context.hpp"
#include <unordered_map>

namespace llvm {

class Module {
private:
    std::string name;
    LLVMContext *context;
    std::unordered_map<std::string, std::unique_ptr<Function>> functions;
    std::unordered_map<std::string, std::unique_ptr<GlobalVariable>> global_variables;
public:
    Module(const std::string& name, LLVMContext *context) : name(name), context(context) {}
};

}