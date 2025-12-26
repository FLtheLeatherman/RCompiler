#!/usr/bin/env python3
"""
自动化语义分析测试脚本
读取 test/sema1_result.txt 中的测试点，使用 run_test 程序进行测试，并比较结果
"""

import subprocess
import os
import sys
from pathlib import Path

def read_test_cases(result_file):
    """读取测试用例文件，返回 (test_name, expected_result) 列表"""
    test_cases = []
    try:
        with open(result_file, 'r') as f:
            for line in f:
                line = line.strip()
                if line:
                    parts = line.split()
                    if len(parts) >= 2:
                        test_name = parts[0]
                        expected_result = int(parts[1])
                        test_cases.append((test_name, expected_result))
    except FileNotFoundError:
        print(f"错误: 找不到测试结果文件 {result_file}")
        sys.exit(1)
    except Exception as e:
        print(f"错误: 读取测试结果文件时出错: {e}")
        sys.exit(1)
    
    return test_cases

def run_single_test(test_name):
    """运行单个测试，返回 (success, error_occurred)"""
    try:
        # 运行测试程序
        result = subprocess.run(
            ['./run_test', test_name],
            cwd='build',
            capture_output=True,
            text=True,
            timeout=30  # 30秒超时
        )
        
        # 检查退出码
        if result.returncode != 0:
            # 程序异常退出，认为是编译错误
            return (False, True)
        else:
            # 程序正常退出，认为是编译通过
            return (True, False)
            
    except subprocess.TimeoutExpired:
        print(f"警告: 测试 {test_name} 超时")
        return (False, True)
    except Exception as e:
        print(f"警告: 运行测试 {test_name} 时出错: {e}")
        return (False, True)

def main():
    # 获取脚本所在目录
    script_dir = Path(__file__).parent
    os.chdir(script_dir.parent)
    
    result_file = "test/sema1_result.txt"
    
    print("=" * 80)
    print("RCompiler 语义分析测试自动化脚本")
    print("=" * 80)
    
    # 读取测试用例
    test_cases = read_test_cases(result_file)
    total_tests = len(test_cases)
    
    print(f"读取到 {total_tests} 个测试用例")
    print()
    
    # 统计变量
    passed_tests = 0
    failed_tests = 0
    consistent_tests = 0
    
    # 测试结果表头
    print(f"{'测试点':<15} {'标准结果':<10} {'我的结果':<10} {'一致性':<10} {'状态'}")
    print("-" * 70)
    
    # 逐个测试
    for i, (test_name, expected_result) in enumerate(test_cases, 1):
        # 运行测试
        test_passed, error_occurred = run_single_test(test_name)
        
        # 确定测试结果
        if error_occurred:
            my_result = -1  # 编译失败
            status = "编译错误"
        else:
            my_result = 0   # 编译通过
            status = "编译通过"
        
        # 检查结果一致性
        is_consistent = (my_result == expected_result)
        if is_consistent:
            consistent_tests += 1
            consistency_str = "✓ 一致"
        else:
            consistency_str = "✗ 不一致"
        
        # 显示结果
        expected_str = "通过" if expected_result == 0 else "失败"
        my_str = "通过" if my_result == 0 else "失败"
        
        print(f"{test_name:<15} {expected_str:<10} {my_str:<10} {consistency_str:<10} {status}")
        
        # 统计
        if test_passed:
            passed_tests += 1
        else:
            failed_tests += 1
    
    print("-" * 70)
    print()
    
    # 输出统计结果
    print("=" * 80)
    print("测试统计结果")
    print("=" * 80)
    print(f"总测试数量: {total_tests}")
    print(f"一致测试数量: {consistent_tests}")
    print(f"不一致测试数量: {total_tests - consistent_tests}")
    print(f"一致率: {consistent_tests / total_tests * 100:.2f}%")
    print()
    
    if consistent_tests == total_tests:
        print("🎉 所有测试结果都与标准答案一致！")
        return 0
    else:
        print(f"⚠️  有 {total_tests - consistent_tests} 个测试结果与标准答案不一致")
        return 1

if __name__ == "__main__":
    sys.exit(main())