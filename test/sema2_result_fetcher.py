#!/usr/bin/env python3
import json
import sys
import os
from pathlib import Path

def main():
    """从 global.json 抓取测试用例的编译结果并输出到 sema1_result.txt"""
    
    # JSON 文件路径
    json_file_path = ".RCompiler-Testcases/semantic-2/global.json"
    
    # 输出文件路径
    output_file_path = "test/sema2_result.txt"
    
    try:
        # 读取 JSON 文件
        with open(json_file_path, 'r', encoding='utf-8') as f:
            data = json.load(f)
        
        # 准备结果数据
        results = []
        
        # 遍历所有测试用例
        for test_case in data:
            # 只处理活跃的测试用例
            if test_case.get('active', True):
                name = test_case['name']
                compile_exit_code = test_case.get('compileexitcode', -1)
                
                # 添加到结果列表
                results.append(f"{name} {compile_exit_code}")
        
        # 确保 test 目录存在
        os.makedirs("test", exist_ok=True)
        
        # 写入输出文件
        with open(output_file_path, 'w', encoding='utf-8') as f:
            for result in results:
                f.write(result + '\n')
        
        print(f"成功抓取 {len(results)} 个测试用例的结果")
        print(f"结果已保存到: {output_file_path}")
        
        return 0
        
    except FileNotFoundError:
        print(f"错误: 找不到文件 {json_file_path}")
        return 1
    except json.JSONDecodeError as e:
        print(f"错误: JSON 解析失败 - {e}")
        return 1
    except Exception as e:
        print(f"错误: {e}")
        return 1

if __name__ == "__main__":
    sys.exit(main())