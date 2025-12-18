#!/usr/bin/env python3
import json
import subprocess
import sys
import os
from pathlib import Path

def run_command(cmd):
    """运行命令并返回退出码"""
    try:
        result = subprocess.run(cmd, shell=True, capture_output=True, text=True, timeout=30)
        return result.returncode
    except subprocess.TimeoutExpired:
        return -1
    except Exception:
        return -1

def load_expected_results():
    """从 global.json 加载预期结果"""
    try:
        with open(".RCompiler-Testcases/semantic-1/global.json", 'r', encoding='utf-8') as f:
            data = json.load(f)
        
        expected = {}
        for test_case in data:
            if test_case.get('active', True):  # 只处理活跃的测试用例
                name = test_case['name']
                compile_exit_code = test_case.get('compileexitcode', -1)
                # 转换：如果 compileexitcode 是 0，我们的 parser 应该返回 0（成功）
                # 如果 compileexitcode 是 -1，我们的 parser 应该返回 -1（失败）
                expected[name] = 0 if compile_exit_code == 0 else -1
        
        return expected
    except Exception as e:
        print(f"错误: 无法读取 global.json: {e}")
        return {}

def run_parser_test(test_name):
    """运行单个 parser 测试"""
    executable = "./build/parser_test"
    if not os.path.exists(executable):
        print(f"错误: 可执行文件不存在: {executable}")
        return -1
    
    exit_code = run_command(f"{executable} {test_name}")
    # 将任何非 0 的退出码都转换为 -1
    return 0 if exit_code == 0 else -1

def main():
    print("=== Parser 简化测试脚本 ===")
    
    # 1. 加载预期结果
    print("正在加载预期结果...")
    expected_results = load_expected_results()
    if not expected_results:
        print("错误: 无法加载预期结果")
        sys.exit(1)
    
    print(f"加载了 {len(expected_results)} 个预期结果")
    
    # 2. 测试几个示例用例
    test_cases = ["basic21"]
    
    print(f"\n开始测试 {len(test_cases)} 个示例用例...")
    results = {}
    
    for test_name in test_cases:
        print(f"测试 {test_name}...", end=" ")
        
        if test_name not in expected_results:
            print("SKIP (不在预期结果中)")
            continue
        
        result = run_parser_test(test_name)
        results[test_name] = result
        
        expected = expected_results[test_name]
        if result == expected:
            print("PASS")
        else:
            print(f"FAIL (实际: {result}, 预期: {expected})")
    
    # 3. 生成简单报告
    print("\n=== 测试结果 ===")
    passed = 0
    for test_name, result in results.items():
        expected = expected_results.get(test_name, 'UNKNOWN')
        status = "PASS" if result == expected else "FAIL"
        print(f"{test_name}: {status} (实际: {result}, 预期: {expected})")
        if result == expected:
            passed += 1
    
    print(f"\n通过: {passed}/{len(results)}")
    
    if passed == len(results):
        print("🎉 所有测试通过!")
    else:
        print("❌ 有测试失败")

if __name__ == "__main__":
    main()