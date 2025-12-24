#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include "parser/astnode.hpp"

// 前向声明
class ASTNode;

// ConstValue 基类
class ConstValue {
protected:
    std::shared_ptr<ASTNode> expression_node;

public:
    ConstValue(std::shared_ptr<ASTNode> node);
    virtual ~ConstValue() = default;
    
    // 获取对应的 AST 节点
    std::shared_ptr<ASTNode> getExpressionNode() const;
    
    // 获取值的类型（用于类型检查）
    virtual std::string getValueType() const = 0;
    
    // 获取字符串表示（用于调试）
    virtual std::string toString() const = 0;
    
    // 类型检查方法
    virtual bool isInt() const { return false; }
    virtual bool isBool() const { return false; }
    virtual bool isChar() const { return false; }
    virtual bool isString() const { return false; }
    virtual bool isStruct() const { return false; }
    virtual bool isEnum() const { return false; }
};

// 整型常量值
class ConstValueInt : public ConstValue {
private:
    int value;

public:
    ConstValueInt(int value, std::shared_ptr<ASTNode> node);
    
    int getValue() const;
    void setValue(int value);
    
    std::string getValueType() const override;
    std::string toString() const override;
    
    bool isInt() const override { return true; }
};

// 布尔常量值
class ConstValueBool : public ConstValue {
private:
    bool value;

public:
    ConstValueBool(bool value, std::shared_ptr<ASTNode> node);
    
    bool getValue() const;
    void setValue(bool value);
    
    std::string getValueType() const override;
    std::string toString() const override;
    
    bool isBool() const override { return true; }
};

// 字符常量值
class ConstValueChar : public ConstValue {
private:
    char value;

public:
    ConstValueChar(char value, std::shared_ptr<ASTNode> node);
    
    char getValue() const;
    void setValue(char value);
    
    std::string getValueType() const override;
    std::string toString() const override;
    
    bool isChar() const override { return true; }
};

// 字符串常量值
class ConstValueString : public ConstValue {
private:
    std::string value;

public:
    ConstValueString(const std::string& value, std::shared_ptr<ASTNode> node);
    
    const std::string& getValue() const;
    void setValue(const std::string& value);
    
    std::string getValueType() const override;
    std::string toString() const override;
    
    bool isString() const override { return true; }
};

// 结构体常量值
class ConstValueStruct : public ConstValue {
private:
    std::string struct_name;
    std::unordered_map<std::string, std::shared_ptr<ConstValue>> fields;

public:
    ConstValueStruct(const std::string& struct_name, std::shared_ptr<ASTNode> node);
    
    const std::string& getStructName() const;
    void setStructName(const std::string& name);
    
    // 字段操作
    void setField(const std::string& field_name, std::shared_ptr<ConstValue> value);
    std::shared_ptr<ConstValue> getField(const std::string& field_name) const;
    bool hasField(const std::string& field_name) const;
    const std::unordered_map<std::string, std::shared_ptr<ConstValue>>& getFields() const;
    
    std::string getValueType() const override;
    std::string toString() const override;
    
    bool isStruct() const override { return true; }
};

// 枚举常量值
class ConstValueEnum : public ConstValue {
private:
    std::string enum_name;
    std::string variant_name;

public:
    ConstValueEnum(const std::string& enum_name, const std::string& variant_name, std::shared_ptr<ASTNode> node);
    
    const std::string& getEnumName() const;
    void setEnumName(const std::string& name);
    
    const std::string& getVariantName() const;
    void setVariantName(const std::string& name);
    
    std::string getValueType() const override;
    std::string toString() const override;
    
    bool isEnum() const override { return true; }
};
