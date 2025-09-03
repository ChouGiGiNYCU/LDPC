import subprocess
import re

# 用正常编译（不加 -fsyntax-only）来获得提示
cmd = ["g++", "-std=c++11", "-fsyntax-only","test.cpp"]
res = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

# 正则匹配 “did you forget to ‘#include <…>’？”
pattern = re.compile(r"did you forget to ['‘\"]#include\s*(<[^>]+>)['’\"]")

suggested_headers = set()
for line in res.stderr.splitlines():
    m = pattern.search(line)
    if m:
        suggested_headers.add(m.group(1))

if suggested_headers:
    print("GCC 建议补充头文件：")
    for hdr in suggested_headers:
        print(f"  #include {hdr}")
else:
    print("没有检测到自动补充 include 的建议。")
