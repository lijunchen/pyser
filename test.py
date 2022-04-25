import os
from pathlib import Path

files = os.listdir("./test")

pyser = Path("./build/pyser")
p = Path("./test")

for file in p.glob("*.py"):
    script = f"cat {file} | {pyser} > ./tmp/{file.name}.out"
    print(script)
    os.system(script)
    script = f"python3.10 -m astpretty --no-show-offsets {file} > ./tmp/{file.name}.ans"
    os.system(script)
