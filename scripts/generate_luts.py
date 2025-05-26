import math
import os
import re

HEADER_FILES = ["include/fixed_math.hpp", "include/fixed.hpp"]
OUTPUT_FILE = "include/generated_luts.hpp"

os.chdir(os.path.join(os.path.dirname(__file__), '..'))

# Read defines from header file
def parse_defines(filenames):
    defines = {}
    define_pattern = re.compile(r"#define\s+(\w+)\s+([\d]+)")

    for filename in filenames:
        with open(filename, "r") as f:
            for line in f:
                match = define_pattern.match(line)
                if match:
                    key, val = match.groups()
                    defines[key] = int(val)

    return defines

def write_fixed(val, scale):
    return str(int(round(val * scale)))

def gen_sin_table(size, scale):
    out = [f"static const int16_t sinTable[{size}] = {{"]
    for i in range(size):
        angle = 2.0 * math.pi * i / size
        val = write_fixed(math.sin(angle), scale)
        out.append(f"    {val},")
    out.append("};\n")
    return "\n".join(out)

def gen_atan2_table(size, output_max):
    scale = output_max / (2.0 * math.pi)  # radians → 0-255
    out = [f"static const uint16_t atan2Table[{size}] = {{"]
    for i in range(size):
        ratio = (i / size) * 2.0  # [0, 2.0)
        val = int(round(math.atan(ratio) * scale))
        out.append(f"    {val},")
    out.append("};\n")
    return "\n".join(out)

def gen_sqrt_table(size, scale):
    out = [f"static const int16_t sqrtLUT[{size}] = {{"]
    for i in range(size):
        val = write_fixed(math.sqrt(i / 16.0), scale)
        out.append(f"    {val},")
    out.append("};\n")
    return "\n".join(out)

def gen_log_table(size, scale):
    out = [f"static const int16_t logLUT[{size}] = {{"]
    for i in range(size):
        if i == 0:
            out.append("    0,")
        else:
            val = write_fixed(math.log(i / 16.0), scale)
            out.append(f"    {val},")
    out.append("};\n")
    return "\n".join(out)

def main():
    defines = parse_defines(HEADER_FILES)

    FIXED_SHIFT = defines.get("FIXED_SHIFT", 4)
    FIXED_SCALE = 1 << FIXED_SHIFT

    SIN_SIZE = defines.get("TRIG_RESOLUTION", 256)
    ATAN_SIZE = defines.get("ATAN2_LUT_SIZE", 256)
    SQRT_SIZE = defines.get("LUT_RESOLUTION", 256)
    LOG_SIZE  = defines.get("LUT_RESOLUTION", 256)

    with open(OUTPUT_FILE, "w") as f:
        f.write("#pragma once\n\n")
        f.write("// Auto-generated lookup tables based on fixed_math.hpp\n\n")
        f.write(gen_sin_table(SIN_SIZE, FIXED_SCALE))
        f.write(gen_atan2_table(ATAN_SIZE, ATAN_SIZE))
        f.write(gen_sqrt_table(SQRT_SIZE, FIXED_SCALE))
        f.write(gen_log_table(LOG_SIZE, FIXED_SCALE))
        print(f"{OUTPUT_FILE} generated.")

if __name__ == "__main__":
    main()
