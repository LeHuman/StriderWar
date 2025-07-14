import argparse
import json
import sys

HEADER_FORMAT = """
#pragma once

#include <stddef.h>
#include <stdint.h>

namespace sprite {{

{lines}

}} // namespace sprite
"""

DECLARE_FORMAT = "static const size_t {name} = {id};"

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        prog='Update Sprite Map',
        description='Update sprite maps ids in a C++ header file')

    parser.add_argument('input_json', help="The input JSON to use")
    parser.add_argument('output_header', help="The output header to write to")

    args = parser.parse_args(sys.argv[1:])

    with open(args.input_json, mode="r", encoding="utf-8") as file:
        mapping: dict[str, int] = json.load(file)

    lines = [DECLARE_FORMAT.format(name=name, id=id) for name, id in mapping.items()]
    text = HEADER_FORMAT.format(lines='\n'.join(lines))

    with open(args.output_header, mode="w", encoding="utf-8") as file:
        file.write(text)
