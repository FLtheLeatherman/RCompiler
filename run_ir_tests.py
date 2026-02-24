#!/usr/bin/env python3
"""
批量运行 RCompiler 测试脚本
读取 data.txt 中的测试点名称，依次调用 run_ir_test.py，
并将每个测试点的结果（答案正确/答案错误/中途出错）输出到 result.txt
"""

import os
import sys
import subprocess
import argparse

def read_testcases(data_file):
    """从 data.txt 读取测试点名称，忽略空行和首尾空白"""
    testcases = []
    try:
        with open(data_file, 'r', encoding='utf-8') as f:
            for line in f:
                line = line.strip()
                if line and not line.startswith('#'):  # 可选：忽略以#开头的注释行
                    testcases.append(line)
    except FileNotFoundError:
        print(f"错误: 数据文件 {data_file} 不存在。")
        sys.exit(1)
    return testcases

def run_single_test(test_name, runner_script):
    """运行单个测试，返回状态字符串"""
    # 构建命令：尝试直接执行脚本（需有执行权限），否则用 python 解释器运行
    if os.access(runner_script, os.X_OK):
        cmd = [runner_script, test_name]
    else:
        cmd = [sys.executable, runner_script, test_name]  # 使用当前 Python 解释器

    try:
        # 捕获 stdout 和 stderr
        result = subprocess.run(cmd, capture_output=True, text=True, timeout=60)
        stdout = result.stdout
        stderr = result.stderr
        returncode = result.returncode
    except subprocess.TimeoutExpired:
        return "运行超时"
    except Exception as e:
        return f"调用异常: {str(e)}"

    # 根据返回码和输出判断状态
    if returncode == 0:
        # 成功退出，检查是否真的输出成功信息（防止误判）
        if "测试成功：输出一致" in stdout:
            return "答案正确"
        else:
            # 按理说不应该发生，但以防万一
            return "中途出错（未知成功输出）"
    else:
        # 非零退出，检查是否是答案错误
        if "测试失败：输出不一致" in stdout or "测试失败：输出不一致" in stderr:
            return "答案错误"
        else:
            # 其他错误
            return "中途出错"

def main():
    parser = argparse.ArgumentParser(description='批量运行 RCompiler 测试')
    parser.add_argument('--data', default='data.txt', help='测试点列表文件 (默认: data.txt)')
    parser.add_argument('--output', default='result.txt', help='结果输出文件 (默认: result.txt)')
    parser.add_argument('--runner', default='run_ir_test.py', help='测试运行脚本 (默认: run_ir_test.py)')
    args = parser.parse_args()

    # 获取脚本所在目录，以便找到相关文件
    script_dir = os.path.dirname(os.path.abspath(__file__))
    data_file = os.path.join(script_dir, args.data)
    runner_script = os.path.join(script_dir, args.runner)
    output_file = os.path.join(script_dir, args.output)

    # 检查运行脚本是否存在
    if not os.path.isfile(runner_script):
        print(f"错误: 运行脚本 {runner_script} 不存在。")
        sys.exit(1)

    # 读取测试点
    testcases = read_testcases(data_file)
    if not testcases:
        print("警告: 测试点列表为空，没有可运行的测试。")
        # 仍然生成空的结果文件
        with open(output_file, 'w', encoding='utf-8') as f:
            f.write("")
        return

    results = []
    print(f"开始批量测试，共 {len(testcases)} 个测试点...")
    for idx, test in enumerate(testcases, 1):
        print(f"[{idx}/{len(testcases)}] 正在测试: {test} ...")
        status = run_single_test(test, runner_script)
        results.append((test, status))
        print(f"    -> {status}")

    # 写入结果文件
    with open(output_file, 'w', encoding='utf-8') as f:
        for test, status in results:
            f.write(f"{test}: {status}\n")

    print(f"\n测试完成，结果已保存至 {output_file}")

if __name__ == '__main__':
    main()