#!/usr/bin/env python3
import subprocess
import sys
import os
from pathlib import Path

def run_command(cmd):
    """运行命令并返回退出码"""
    try:
        result = subprocess.run(cmd, shell=True, capture_output=True, text=True, timeout=60)
        return result.returncode
    except subprocess.TimeoutExpired:
        return -1
    except Exception:
        return -1

def compile_parser_test():
    """编译 parser_test.cpp"""
    print("正在编译 parser_test.cpp...")
    build_dir = Path("build")
    if not build_dir.exists():
        build_dir.mkdir()
    
    # 运行 cmake 和 make
    if run_command("cd build && cmake ..") != 0:
       print("错误: CMake 配置失败")
       return False
    
    if run_command("cd build && make parser_test") != 0:
       print("错误: 编译 parser_test 失败")
       return False
    
    print("编译成功!")
    return True

def load_expected_results():
    """从 sema1_result.txt 加载预期结果"""
    try:
        expected = {}
        with open("test/sema1_result.txt", 'r', encoding='utf-8') as f:
            for line in f:
                line = line.strip()
                if line:
                    parts = line.split()
                    if len(parts) >= 2:
                        name = parts[0]
                        exit_code = int(parts[1])
                        expected[name] = exit_code
        
        return expected
    except Exception as e:
        print(f"错误: 无法读取 sema1_result.txt: {e}")
        return {}

def get_test_cases():
    """从 sema1_result.txt 获取所有测试用例名称"""
    try:
        test_cases = []
        with open("test/sema1_result.txt", 'r', encoding='utf-8') as f:
            for line in f:
                line = line.strip()
                if line:
                    parts = line.split()
                    if len(parts) >= 2:
                        test_cases.append(parts[0])
        
        return sorted(test_cases)
    except Exception as e:
        print(f"错误: 无法读取测试用例列表: {e}")
        return []

def run_parser_test(test_name):
    """运行单个 parser 测试"""
    executable = "./build/parser_test"
    if not os.path.exists(executable):
        print(f"错误: 可执行文件不存在: {executable}")
        return -1
    
    return run_command(f"{executable} {test_name}")

def generate_result_report(results, expected_results):
    """生成测试结果报告"""
    report_lines = [
        "# Parser 测试结果报告",
        "",
        f"总测试用例数: {len(results)}",
        f"通过: {sum(1 for r in results.values() if r == expected_results.get(r, 'UNKNOWN'))}",
        f"失败: {sum(1 for r in results.values() if r != expected_results.get(r, 'UNKNOWN'))}",
        "",
        "## 详细结果",
        "",
        "| 测试用例 | 实际结果 | 预期结果 | 状态 |",
        "|---------|---------|---------|------|",
    ]
    
    for test_name in sorted(results.keys()):
        actual = results[test_name]
        expected = expected_results.get(test_name, 'UNKNOWN')
        
        if actual == expected:
            status_display = "PASS"
        else:
            status_display = "FAIL"
        
        report_lines.append(f"| {test_name} | {actual} | {expected} | {status_display} |")
    
    # 添加失败用例的详细信息
    failed_cases = [name for name, result in results.items() 
                   if result != expected_results.get(name, 'UNKNOWN')]
    
    if failed_cases:
        report_lines.extend([
            "",
            "## 失败用例详情",
            ""
        ])
        
        for test_name in failed_cases:
            actual = results[test_name]
            expected = expected_results.get(test_name, 'UNKNOWN')
            report_lines.append(f"### {test_name}")
            report_lines.append(f"- 实际结果: {actual}")
            report_lines.append(f"- 预期结果: {expected}")
            report_lines.append("")
    
    return "\n".join(report_lines)

def main():
    print("=== Parser 自动化测试脚本 ===")
    
    # 1. 编译 parser_test.cpp
    if not compile_parser_test():
        sys.exit(1)
    
    # 2. 加载预期结果
    print("\n正在加载预期结果...")
    expected_results = load_expected_results()
    if not expected_results:
        print("错误: 无法加载预期结果")
        sys.exit(1)
    
    print(f"加载了 {len(expected_results)} 个预期结果")
    
    # 3. 获取所有测试用例
    print("\n正在获取测试用例列表...")
    test_cases = get_test_cases()
    if not test_cases:
        print("错误: 没有找到测试用例")
        sys.exit(1)
    
    print(f"找到 {len(test_cases)} 个测试用例")
    
    # 4. 运行测试
    print("\n开始运行测试...")
    results = {}
    
    for i, test_name in enumerate(test_cases, 1):
        print(f"[{i}/{len(test_cases)}] 测试 {test_name}...", end=" ")
        
        result = run_parser_test(test_name)
        if result == 255:
            result = -1
        results[test_name] = result
        
        # 立即与标准结果比较
        expected = expected_results.get(test_name, 'UNKNOWN')
        
        if result == expected:
            print("PASS")
        else:
            print(f"FAIL (实际: {result}, 预期: {expected})")
    
    # 5. 生成报告
    print("\n正在生成测试报告...")
    report = generate_result_report(results, expected_results)
    
    # 确保测试目录存在
    os.makedirs("test", exist_ok=True)
    
    # 写入报告文件
    with open("test/sema1_test_result.md", "w", encoding="utf-8") as f:
        f.write(report)
    
    print("测试报告已保存到: test/sema1_test_result.md")
    
    # 6. 输出摘要
    passed = sum(1 for name, result in results.items() 
                if result == expected_results.get(name, 'UNKNOWN'))
    total = len(results)
    
    print(f"\n=== 测试完成 ===")
    print(f"通过: {passed}/{total}")
    print(f"成功率: {passed/total*100:.1f}%")
    
    if passed == total:
        print("🎉 所有测试通过!")
        sys.exit(0)
    else:
        print("❌ 有测试失败")
        sys.exit(1)

if __name__ == "__main__":
    main()