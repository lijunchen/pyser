import os
import sys
from pathlib import Path
import ast
from ast import *
from astpretty import pformat

files = os.listdir("./test")

if os.name == 'nt':
    pyser = Path("./build/Debug/pyser")
else:
    pyser = Path("./build/pyser")
p = Path("./test")

def pyser_output(filename: Path) -> str:
    script = f"cat {filename} | {pyser} > ./tmp/{filename.name}.out"
    # print(f"{script}")
    os.system(script)
    with open(f"./tmp/{filename.name}.out") as fp:
        source = fp.read()
    try:
        tree = eval(source)
        text = pformat(tree, show_offsets=False)
    except Exception as e:
        print(e)
        text = ""
    with open(f"./tmp/{filename.name}.eval", "w") as fp:
        fp.write(text)
    return text

def answer(filename: Path) -> str:
    with open(f"{filename}") as fp:
        source = "".join(fp.readlines())
    tree = ast.parse(source)
    text = pformat(tree, show_offsets=False)
    with open(f"./tmp/{filename.name}.ans", "w") as fp:
        fp.write(text)
    return text


def test():
    p = Path("./test")
    files = sorted(p.glob("*.py"))
    total = len(files)
    succ = 0
    for file in files:
        out = pyser_output(file)
        ans = answer(file)
        if out == ans:
            print(f"{str(file):<48} succ")
            succ += 1
        else:
            print(f"{str(file):<48} failed")
    if succ != total:
        sys.exit(-1)
        

if __name__ == "__main__":
    test()
