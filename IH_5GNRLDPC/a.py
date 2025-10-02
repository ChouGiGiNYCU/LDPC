import subprocess
import time
import re
import sys
from pathlib import Path

# === 使用前請修改這些參數 ===
EXE_PATH = r"C:\Users\USER\Desktop\LDPC\IH_5GNRLDPC\5GLDPC.exe"   # 你的 .exe 路徑

WORK_DIR = None                              # 若需在特定資料夾執行，填入路徑字串；否則 None
                              # 第二次要輸入的數值（字串）



def rename_in_place(path, new_name):
    p = Path(path)
    p.rename(p.with_name(new_name if Path(new_name).suffix else new_name + p.suffix))

def main():
    exe = Path(EXE_PATH)
    if not exe.exists():
        print(f"[ERROR] 找不到 EXE：{exe}")
        sys.exit(1)

    for info_bits in range(300,2001):
        for BG_idx in range(1,3):

            # 建立子行程
            # text=True 讓 stdin/stdout 用字串模式；stderr 併到 stdout 便於統一讀取
            proc = subprocess.Popen(
                [str(exe)],
                cwd=WORK_DIR,
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                text=True,
                bufsize=1  # 行緩衝
            )

    
            def safe_write_line(line: str):
                """安全寫入一行到子行程 stdin"""
                if proc.stdin:
                    proc.stdin.write(line)
                    proc.stdin.flush()

            try:
                # 視需求：若 .exe 會先印提示再等待輸入，可稍等一下讓提示印出來
                # （若不需要可移除 sleep）
                time.sleep(0.2)

                # 寫入第一個數值 + Enter
                safe_write_line(f"{info_bits}\n")

                # 若程式會在第一個輸入後印出新提示，可再等一下
                time.sleep(0.2)

                # 寫入第二個數值 + Enter
                safe_write_line(f"{BG_idx}\n")

                # 關閉 stdin（有些程式需要 EOF 才會結束）
                if proc.stdin: proc.stdin.close()

                # 即時讀取輸出直到程式結束
                # 這段會把印到 cmd 的每一行都收集起來
                all_output_lines =[]
                if proc.stdout:
                    for line in proc.stdout:
                        sys.stdout.write(f"{line}")  # 若想同步顯示在你自己的 console，就保留；不想顯示可拿掉
                        all_output_lines.append(line)

                return_code = proc.wait(timeout=120)  # 設定最多等 120 秒（可調）
                if return_code != 0:
                    print(f"[WARN] 子行程結束但 return code = {return_code}")

            except subprocess.TimeoutExpired:
                print("[ERROR] 子行程逾時，嘗試終止...")
                proc.kill()
            except Exception as e:
                print(f"[ERROR] 執行過程發生例外：{e}")
                try:
                    proc.kill()
                except Exception:
                     pass
                continue
            
            pattern = r"(?i)\bZ\s*_?\s*c\b\s*[:：=]?\s*\(?\s*([+-]?\d+(?:\.\d+)?)"
            m = re.search(pattern, all_output_lines[1], flags=re.IGNORECASE)
            
            if m:
                zc = int(m.group(1))
                print(f"zc : {zc}")
            else:
                continue
            # get col
            CodeWord_length = int(all_output_lines[-1].split(":")[-1])
            print(f"CodeWord_length : {CodeWord_length} | code_rate : {info_bits/CodeWord_length}")
            if info_bits/CodeWord_length <= 0.33:
                rename_in_place("H.txt",f"5G_H_{CodeWord_length}_{info_bits}_BG{BG_idx}_Z{zc}.txt")
            
            


if __name__ == "__main__":
    main()
