import argparse
import os
import sys
from typing import Any
from itertools import batched

from PIL import Image

COLOR_TO_INDEX = {
    '000000': 0,  # Black
    # CGA2
    '00C400': 1, 'C40000': 2, 'C47E00': 3,
    '4EDC4E': 1, 'DC4E4E': 2, 'F3F34E': 3,
    # CGA1
    '00C4C4': 1, 'C400C4': 2, 'C4C4C4': 3,
    '4EF3F3': 1, 'F34EF3': 2, 'FFFFFF': 3,
    # DEV Legacy
    '198c0f': 1, 'df0025': 2, 'd4a714': 3,
}

COLOR_INDEX_STR = [f'0x{v}' for v in COLOR_TO_INDEX]
COLOR_INDEX_STR = [', '.join(g) for g in batched(COLOR_INDEX_STR[1:], 3)] + [f'{COLOR_INDEX_STR[0]}']
COLOR_INDEX_STR = '\n    '.join(COLOR_INDEX_STR)

COLOR_TO_INDEX = {
    tuple(int(hx[i:i+2], 16) for i in (0, 2, 4)): val
    for hx, val in COLOR_TO_INDEX.items()
}

VALID_COLORS_STR = f"""
Valid colors:
    {COLOR_INDEX_STR}
"""

VALID_FILE_SIZES_STR = """
Valid file sizes and output:
    320x200 : .cga
    320x400 : .cgi
    640x200 : .hga
    640x400 : .hgi
"""


def nearest_match(color):
    if color in COLOR_TO_INDEX:
        return COLOR_TO_INDEX[color]
    raise ValueError(f"Unknown color {color}")


def convert_image_to_hga(input_png, output_bin, interlaced=False):
    img = Image.open(input_png).convert('1')

    if interlaced and img.size != (640, 400):
        raise ValueError("Interlaced Image must be 640x400")
    if not interlaced and img.size != (640, 200):
        raise ValueError("Image must be 640x200")

    pixels: Any = img.load()
    generator = range(0, 400, 2) if interlaced else range(200)

    with open(output_bin, "wb") as f:
        for y in generator:
            for x in range(0, 640, 8):
                byte = 0
                for i in range(8):
                    if pixels[x + i, y] == 255:
                        byte |= (1 << (7 - i))
                f.write(bytes([byte]))

    print(f"Saved HGA {'interlaced ' if interlaced else ''}image to {output_bin}")


def convert_image_to_cga(input_png, output_bin, interlaced=False):
    img = Image.open(input_png)

    if interlaced and img.size != (320, 400):
        raise ValueError("Interlaced Image must be 320x400")
    if not interlaced and img.size != (320, 200):
        raise ValueError("Image must be 320x200")

    pixels: Any = img.load()
    generator = range(0, 400, 2) if interlaced else range(200)

    with open(output_bin, "wb") as f:
        for y in generator:
            for x in range(0, 320, 4):
                byte = 0
                for i in range(4):
                    color = pixels[x + i, y]
                    if len(color) > 3:
                        if (color[3] <= (255/2)):
                            color = (0, 0, 0)
                        else:
                            color = (color[0], color[1], color[2])
                    index = nearest_match(color)
                    byte |= (index & 0x03) << (6 - 2 * i)
                f.write(bytes([byte]))

    print(f"Saved CGA {'interlaced ' if interlaced else ''}image to {output_bin}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        prog=sys.argv[0] if len(sys.argv) >= 1 else "png2cga.py",
        description='Convert PNGs to a CGA Image for the IBM PC JR.',
        epilog=f'NOTE: file extension is automatically output based on file size.\n{VALID_FILE_SIZES_STR}\n{VALID_COLORS_STR}',
        formatter_class=argparse.RawDescriptionHelpFormatter)

    parser.add_argument('input', help="The input.png to convert")
    parser.add_argument('output_dir', help="The output directory to spit out the CGA image")

    args = parser.parse_args(sys.argv[1:])

    input_path = args.input
    raw_filepath, _ = os.path.splitext(input_path)
    filename = os.path.split(raw_filepath)[-1]
    image = Image.open(input_path)

    cga_path = os.path.join(args.output_dir, f"{filename}.cga")
    hga_path = os.path.join(args.output_dir, f"{filename}.hcga")

    match image.size:
        case (320, 200):
            convert_image_to_cga(input_path, cga_path, False)
        case (320, 400):
            convert_image_to_cga(input_path, cga_path, True)
        case (640, 200):
            convert_image_to_hga(input_path, hga_path, False)
        case (640, 400):
            convert_image_to_hga(input_path, hga_path, True)
        case _:
            raise ValueError(f"Invalid image size {image.size}")
