#pragma once

#include "type.hpp"
#include "value.hpp"
#include <cstdint>
#include <unordered_map>

namespace llvm {

class IRBuilder {
private:
    uint32_t temp_count; // 用于生成临时变量的计数器
    std::unordered_map<std::string, uint32_t> label_count; // 用于生成标签的计数器
public:
    IRBuilder() = default;
    
};

}
