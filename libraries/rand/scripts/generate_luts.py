import math
import os
from random import Random
import re

HEADER_FILES = ["include/dos/rand/rand.hpp"]
OUTPUT_FILE = "include/dos/rand/rand_lut.hpp"

INT_MIN = -32768
INT_MAX = 32767

os.chdir(os.path.join(os.path.dirname(__file__), '..'))

# Read const size_t from header file
def parse_consts(filenames):
    consts = {}
    const_pattern = re.compile(r"const\s+size_t\s+(\w+)\s+=\s+([\d]+)")

    for filename in filenames:
        with open(filename, "r", encoding="utf-8") as f:
            for line in f:
                match = const_pattern.match(line)
                if match:
                    key, val = match.groups()
                    consts[key] = int(val)

    return consts


def get_rand_table(size):
    out = [f"static const int16_t random_ints[{size}] = {{"]
    rnd = Random(13378085)
    for _ in range(size):
        val = rnd.randint(INT_MIN, INT_MAX)
        out.append(f"    {val},")
    out.append("};\n")
    return "\n".join(out)


def main():
    consts = parse_consts(HEADER_FILES)

    RANDOM_LUT_SIZE = consts.get("RANDOM_LUT_SIZE", 127)

    with open(OUTPUT_FILE, "w", encoding="utf-8") as f:
        f.write("#pragma once\n\n#include <stdint.h>\n\n")
        f.write("// Auto-generated lookup tables based on rand.hpp\n\n")
        f.write(get_rand_table(RANDOM_LUT_SIZE))
        print(f"{OUTPUT_FILE} generated.")


if __name__ == "__main__":
    main()
