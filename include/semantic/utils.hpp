#pragma once

#include <vector>
#include <array>
#include <string>
#include <cstring>
#include <memory>
#include <parser/astnode.hpp>

inline std::string typeToString(std::shared_ptr<Type> type) {
    if (!type || !type->child) {
        return "unknown";
    }
    
    // 处理不同的类型
    if (auto path_ident = std::dynamic_pointer_cast<PathIdentSegment>(type->child)) {
        return path_ident->identifier;
    } else if (auto ref_type = std::dynamic_pointer_cast<ReferenceType>(type->child)) {
        std::string base_type = typeToString(ref_type->type);
        return (ref_type->is_mutable ? "&mut " : "&") + base_type;
    } else if (auto array_type = std::dynamic_pointer_cast<ArrayType>(type->child)) {
        std::string base_type = typeToString(array_type->type);
        return "[" + base_type + "]";
    } else if (auto unit_type = std::dynamic_pointer_cast<UnitType>(type->child)) {
        return "()";
    }
    
    return "unknown";
}

// 辅助方法：从模式创建变量符号
inline std::shared_ptr<VariableSymbol> createVariableSymbolFromPattern(
    std::shared_ptr<PatternNoTopAlt> pattern, std::shared_ptr<Type> type) {
    
    if (!pattern || !pattern->child) {
        return nullptr;
    }
    
    if (auto ident_pattern = std::dynamic_pointer_cast<IdentifierPattern>(pattern->child)) {
        std::string type_str = typeToString(type);
        return std::make_shared<VariableSymbol>(ident_pattern->identifier, type_str,
                                               ident_pattern->is_ref, ident_pattern->is_mutable);
    } else if (auto ref_pattern = std::dynamic_pointer_cast<ReferencePattern>(pattern->child)) {
        return createVariableSymbolFromPattern(ref_pattern->pattern, type);
    }
    
    return nullptr;
}