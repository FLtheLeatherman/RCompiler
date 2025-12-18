#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "parser/astprinter.hpp"

int main(int argc, char* argv[]) {
    // 检查命令行参数
    if (argc != 2) {
        std::cerr << "用法: " << argv[0] << " <测试名称>" << std::endl;
        std::cerr << "示例: " << argv[0] << " array1" << std::endl;
        return -1;
    }

    std::string test_name = argv[1];
    
    // 构建文件路径
    std::string file_path = ".RCompiler-Testcases/semantic-1/src/" + test_name + "/" + test_name + ".rx";
    
    // 检查文件是否存在
    if (!std::filesystem::exists(file_path)) {
        std::cerr << "错误: 测试文件不存在: " << file_path << std::endl;
        return -1;
    }

    // 读取文件内容
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "错误: 无法打开文件: " << file_path << std::endl;
        return -1;
    }

    std::string source_code;
    std::string line;
    while (std::getline(file, line)) {
        source_code += line + "\n";
    }
    file.close();

    try {
        // 词法分析
        Lexer lexer;
        auto tokens = lexer.lex(source_code);
        
        // 语法分析
        Parser parser(std::move(tokens));
        auto ast = parser.parseCrate();
        // 如果成功解析，返回 0
        return 0;
        
    } catch (const std::exception& e) {
        // 如果解析失败，返回 -1
        return -1;
    } catch (...) {
        // 捕获所有其他异常，返回 -1
        return -1;
    }
}