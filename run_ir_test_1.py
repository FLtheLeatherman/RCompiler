#!/usr/bin/env python3
"""
RCompiler 正确性测试脚本
用法: ./test_compiler.py [test_name]
默认 test_name 为 comprehensive1
"""

import os
import sys
import subprocess
import argparse
import filecmp

def run_test(test_name):
    # 确定项目根目录（假设脚本从根目录运行）
    project_root = os.getcwd()
    build_dir = os.path.join(project_root, 'build')
    testcases_root = os.path.join(project_root, '.RCompiler-Testcases', 'semantic-1', 'src')
    test_dir = os.path.join(testcases_root, test_name)
    builtin_c = os.path.join(build_dir, 'builtin.c')          # 修改：builtin.c 在 build 目录下
    input_file = os.path.join(test_dir, f'{test_name}.in')
    expected_output_file = os.path.join(test_dir, f'{test_name}.out')

    # 检查必要的文件和目录
    for path, desc in [
        (build_dir, 'build 目录'),
        (builtin_c, 'builtin.c'),
        (input_file, '输入文件'),
        (expected_output_file, '期望输出文件')
    ]:
        if not os.path.exists(path):
            print(f"错误: {desc} 不存在: {path}")
            sys.exit(1)

    # 记录待清理的临时文件（绝对路径）
    temp_files = [
        os.path.join(build_dir, 'output.ll'),
        os.path.join(build_dir, 'output'),
        os.path.join(build_dir, 'my_output.out')
    ]

    try:
        # 切换到 build 目录
        os.chdir(build_dir)
        print(f"切换到目录: {build_dir}")

        # 1. 运行 ir_test 生成 LLVM IR
        print(f"运行: ./ir_test {test_name}")
        subprocess.run(["./ir_test", test_name], check=True,
               stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)

        if not os.path.isfile('output.ll'):
            print("错误: ir_test 未生成 output.ll")
            sys.exit(1)

        # 2. 用 clang 编译为可执行文件（使用 build 目录下的 builtin.c）
        print(f"编译: clang output.ll {builtin_c} -o output")
        subprocess.run(["clang", "output.ll", builtin_c, "-o", "output"],
                       check=True, capture_output=True, text=True)

        # 3. 运行可执行文件，重定向输入输出
        print(f"运行: ./output < {input_file} > my_output.out")
        with open(input_file, 'r') as fin:
            with open('my_output.out', 'w') as fout:
                subprocess.run(["./output"], stdin=fin, stdout=fout,
                               stderr=subprocess.PIPE, check=True, text=True)

        # 4. 比较输出
        print("比较输出文件...")
        if filecmp.cmp('my_output.out', expected_output_file, shallow=False):
            print("测试成功：输出一致。")
        else:
            print("测试失败：输出不一致。")
            # 尝试显示 diff 信息
            try:
                diff = subprocess.run(['diff', '-u', 'my_output.out', expected_output_file],
                                      capture_output=True, text=True)
                print("差异如下：")
                print(diff.stdout)
            except Exception:
                pass
            sys.exit(1)

    except subprocess.CalledProcessError as e:
        print(f"命令执行失败: {e}")
        if e.stdout:
            print("标准输出:", e.stdout)
        if e.stderr:
            print("标准错误:", e.stderr)
        sys.exit(1)

    finally:
        # 清理临时文件
        print("清理临时文件...")
        for f in temp_files:
            if os.path.isfile(f):
                os.remove(f)
                print(f"已删除: {f}")

    print("测试完成。")
    # 返回原目录（可选）
    os.chdir(project_root)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='测试 RCompiler 正确性')
    parser.add_argument('test_name', nargs='?', default='comprehensive1',
                        help='测试点名称（默认：comprehensive1）')
    args = parser.parse_args()
    run_test(args.test_name)