import argparse
import os
import sys

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

COLOR_TO_INDEX = {
    tuple(int(hx[i:i+2], 16) for i in (0, 2, 4)): val
    for hx, val in COLOR_TO_INDEX.items()
}


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

    pixels = img.load()
    generator = range(0, 400, 2) if interlaced else range(200)

    with open(output_bin, "wb") as f:
        for y in generator:
            for x in range(0, 640, 8):
                byte = 0
                for i in range(8):
                    if pixels[x + i, y] == 255:
                        byte |= (1 << (7 - i))
                f.write(bytes([byte]))

    print(f"Saved BW {'interlaced ' if interlaced else ''}image to {output_bin}")


def convert_image_to_cga(input_png, output_bin, interlaced=False):
    img = Image.open(input_png)
    if img.size != (320, 200):
        raise ValueError("Image must be 320x200")

    pixels = img.load()

    with open(output_bin, "wb") as f:
        for y in range(200):
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

    print(f"Saved CGA image to {output_bin}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        prog='PNG to CGA Image',
        description='Convert PNGs to a CGA Image for the IBM PC JR.')

    parser.add_argument('input', help="The input.png to convert")
    parser.add_argument('output_dir', help="The output directory to spit out the CGA image")

    args = parser.parse_args(sys.argv[1:])

    if len(sys.argv) < 3:
        print("Usage: png2cga.py input.png output/dir")
        print("Valid file sizes and output:")
        print("  320x200 : .cga")
        print("  320x400 : .cgi")
        print("  640x200 : .hga")
        print("  640x400 : .hgi")
        print("NOTE: file extension is automatically output based on file size")
        sys.exit(1)

    input_path = args.input
    raw_filepath, _ = os.path.splitext(input_path)
    filename = os.path.split(raw_filepath)[-1]
    img = Image.open(input_path)

    cga_path = os.path.join(args.output_dir, f"{filename}.cga")
    hga_path = os.path.join(args.output_dir, f"{filename}.hcga")

    match img.size:
        case (320, 200):
            convert_image_to_cga(input_path, cga_path)
        case (320, 400):
            convert_image_to_cga(input_path, cga_path, True)
        case (640, 400):
            convert_image_to_hga(input_path, hga_path, True)
        case (640, 200):
            convert_image_to_hga(input_path, hga_path, False)
        case _:
            raise ValueError(f"Invalid image size {img.size}")
