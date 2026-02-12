#pragma once

#include "context.hpp"
#include "module.hpp"

namespace llvm {

class IRBuilder {
private:
    LLVMContext *context;
    BasicBlock *current_block;
public:
    IRBuilder(LLVMContext *context) : context(context), current_block(nullptr) {}
    void setInsertPoint(BasicBlock *block);
    BasicBlock* getInsertBlock();
};

}
