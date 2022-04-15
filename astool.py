from dis import dis
from astpretty import pprint
from ast import parse

def ppp(src):
    pprint(parse(src), show_offsets=False)
