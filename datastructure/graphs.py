import re
import os
from graphviz import Source

def render_pairing_heap_images(input_file="output.txt", output_dir="heap_images"):
    # 建立輸出資料夾
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    with open(input_file, "r", encoding="utf-8") as f:
        content = f.read()

    # 使用正則表達式抓取 DOT_START 和 DOT_END 之間的內容
    # dot_pattern = re.compile(r"DOT_START\n(.*?)\nDOT_END", re.DOTALL)
    # 為了保險起見，使用更簡單的字串切割，避免 regex 因為換行符號問題失效
    
    raw_blocks = content.split("DOT_START")
    
    count = 0
    for block in raw_blocks:
        if "DOT_END" not in block:
            continue
            
        # 取出 DOT 程式碼部分
        dot_code = block.split("DOT_END")[0].strip()
        
        # 嘗試從 DOT 代碼中提取標題作為檔名的一部分 (label="xxx")
        title_match = re.search(r'label="(.*?)";', dot_code)
        if title_match:
            safe_title = re.sub(r'[^\w\-]', '_', title_match.group(1)) # 移除不合法檔名字符
            filename = f"{count:02d}_{safe_title}"
        else:
            filename = f"step_{count:02d}"

        # 繪圖
        try:
            src = Source(dot_code)
            output_path = os.path.join(output_dir, filename)
            # render 會自動產生 .pdf (預設) 或 .png
            src.render(filename=output_path, format="png", cleanup=True)
            print(f"Generated: {output_path}.png")
            count += 1
        except Exception as e:
            print(f"Error rendering block {count}: {e}")
            print("Check if Graphviz is installed on your system PATH.")

if __name__ == "__main__":
    render_pairing_heap_images()