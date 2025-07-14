import argparse
from array import array
from concurrent.futures import Future, ThreadPoolExecutor
import glob
import json
import os
import sys
import imageio.v3 as iio
from PIL import Image, ImageDraw
from PIL.Image import Resampling
import numpy as np


DEFAULT_RADIUS = 3
SCREEN_HEIGHT = 200
SCREEN_WIDTH = 320

GENERATE_VIDEO = True
SPRITE_PIXEL_THRESHOLD = 20

BLANK_COLORS = [
    (0, 255, 255, 100),
    (0, 255, 0, 100),
    (0, 0, 255, 100),
    (255, 255, 0, 100),
    (0, 125, 125, 100),
    (0, 125, 0, 100),
    (0, 0, 125, 100),
    (125, 125, 0, 100),
    (0, 255, 125, 100),
    (125, 255, 0, 100),
    (0, 125, 255, 100),
    (255, 125, 0, 100),
]
__NCLR = 0


def get_neighbors(x: int, y: int, radius: int = DEFAULT_RADIUS, update: set[tuple[int, int]] | None = None) -> set[tuple[int, int]]:
    hood = set() if update is None else set(update)

    for py in range(max(y - radius, 0), min(y + radius + 1, SCREEN_HEIGHT)):
        for px in range(max(x - radius, 0), min(x + radius + 1, SCREEN_WIDTH)):
            if (px == x) and (py == y):
                continue
            if (x - px) ** 2 + (y - py) ** 2 <= radius ** 2:
                hood.add((px, py))

    return hood


def alpha_to_color(image, color=(255, 255, 255)):
    """Set all fully transparent pixels of an RGBA image to the specified color.
    This is a very simple solution that might leave over some ugly edges, due
    to semi-transparent areas. You should use alpha_composite_with color instead.

    Source: http://stackoverflow.com/a/9166671/284318

    Keyword Arguments:
    image -- PIL RGBA Image object
    color -- Tuple r, g, b (default 255, 255, 255)

    """
    x = np.array(image)
    r, g, b, a = np.rollaxis(x, axis=-1)
    r[a == 0] = color[0]
    g[a == 0] = color[1]
    b[a == 0] = color[2]
    x = np.dstack([r, g, b, a])
    return Image.fromarray(x, 'RGBA')


def img_to_frame(img):
    return np.array(alpha_to_color(img.resize((SCREEN_WIDTH*4, SCREEN_HEIGHT*4), Resampling.NEAREST)).convert("RGB"))


def next_blank_color():
    global __NCLR
    __NCLR += 1
    __NCLR %= len(BLANK_COLORS)
    return BLANK_COLORS[__NCLR]


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        prog='PNG to CGA Sprites',
        description='Convert PNGs to CGA Sprites for the IBM PC JR')

    parser.add_argument('input', help="The input.png to scrape for sprites")
    parser.add_argument('output_dir', help="The output directory to spit out PNGs and other artifacts")
    parser.add_argument('-m', '--map_dir', help="A directory of PNGs to use for mapping, their names will be used in generating the json if they are a pixel perfect match. NOTE: it is advised to generate sprites without this option, rename them, and then regenerate")

    args = parser.parse_args(sys.argv[1:])

    input_path = args.input
    output_dir = args.output_dir
    map_dir = args.map_dir if os.path.isdir(args.map_dir) else None
    raw_filepath, _ = os.path.splitext(input_path)
    filename = os.path.split(raw_filepath)[-1]
    img = Image.open(input_path)

    os.makedirs(sys.argv[2], exist_ok=True)
    out_path = os.path.join(sys.argv[2], "sprites.bin")
    out_path_png = os.path.join(sys.argv[2], f"{filename}_{{id}}.png")

    if img.size != (SCREEN_WIDTH, SCREEN_HEIGHT):
        raise ValueError(f"Image must be {SCREEN_WIDTH}x{SCREEN_HEIGHT}")

    pixels = img.load()
    sprites = set()
    dropped_sprites = 0
    process_imgs: list[Future] = []

    with ThreadPoolExecutor() as pool:
        if GENERATE_VIDEO:
            process_imgs.append(pool.submit(img_to_frame, img.copy()))

        for y in range(200):
            for x in range(0, 320):
                pixel = pixels[x, y]
                if pixel[3] > (255/2):
                    sprite = [(x, y)]
                    to_visit = get_neighbors(x, y)

                    blank_color = next_blank_color()
                    pixels[x, y] = blank_color

                    while (to_visit):
                        to_add = set()

                        for vx, vy in to_visit:
                            if pixels[vx, vy] == pixel:
                                to_add.update(get_neighbors(vx, vy))
                                sprite.append((vx, vy))
                                pixels[vx, vy] = blank_color
                            elif (pixels[vx, vy][3] <= (255/2)) and (pixels[vx, vy] not in BLANK_COLORS):
                                pixels[vx, vy] = (0, 0, 0, 1)

                        to_visit = to_add
                        if GENERATE_VIDEO:
                            process_imgs.append(pool.submit(img_to_frame, img.copy()))

                    if len(sprite) < SPRITE_PIXEL_THRESHOLD:
                        dropped_sprites += 1
                        if GENERATE_VIDEO:
                            for x, y in sprite:
                                pixels[x, y] = (255, 0, 0, 100)
                            process_imgs.append(pool.submit(img_to_frame, img.copy()))
                    else:
                        sprites.add(tuple(sprite))

    print(f"Unique Sprites: {len(sprites)}")
    print(f"Dropped Sprites: {dropped_sprites}")

    byte_arrays: list[bytes] = []

    # IMPROVE: Ensure 8 byte padding

    mapping_cache = {}

    if map_dir:
        for png in glob.iglob(os.path.join(map_dir, '*.png')):
            root, _ext = os.path.splitext(png)
            _head, tail = os.path.split(root)
            map_img = Image.open(png).tobytes()
            mapping_cache[map_img] = tail

    mapping = {}

    i = 0
    total_bytes = 0
    for sprite in sprites:
        byte_array = bytes()
        offset_map = {}

        for x, y in sprite:
            offset = ((y // 2) * 80) + (0x2000 if (y % 2) else 0x0000) + (x // 4)
            if offset not in offset_map:
                offset_map[offset] = 0
            offset_map[offset] |= 0x3 << (6 - (2 * (x % 4)))

        if not offset_map:
            continue  # Skip empty sprites

        offsets, masks = zip(*offset_map.items())

        # Pad masks
        if len(masks) % 2 != 0:
            masks = list(masks)
            masks.append(0xFF)

        if len(offset_map) > 255:
            print(f"{out_path.format(id=i)} too big")
        else:
            byte_array += array('H', [len(offset_map)]).tobytes()
            byte_array += array('H', offsets).tobytes()
            byte_array += array('B', masks).tobytes()

            print(f"S:{len(byte_arrays)} {len(offset_map)} @ {total_bytes}")
            total_bytes += (len(offset_map) * 3) + 2 + isinstance(masks, list)

            byte_arrays.append(byte_array)

            # Create PNG image for sprite
            sprite_img = Image.new("RGBA", (SCREEN_WIDTH, SCREEN_HEIGHT), (0, 0, 0, 1))
            draw = ImageDraw.Draw(sprite_img)
            for x, y in sprite:
                draw.point((x, y), fill=(255, 255, 255, 255))  # white pixel

            sprite_filename = out_path_png.format(id=i)
            sprite_img.save(sprite_filename)  # Write PNG file

            # Save mapping
            if sprite_img.tobytes() in mapping_cache:
                mapping[mapping_cache[sprite_img.tobytes()]] = i
            else:
                root, _ext = os.path.splitext(sprite_filename)
                _head, tail = os.path.split(root)
                mapping[tail] = i

        i += 1
        if i >= 255:
            print("Too many sprites, stopping")
            break

    with open(os.path.join(output_dir, "map.json"), mode="w", encoding="utf-8") as file:
        json.dump(mapping, file, indent=4)

    print(f"Sprite Size: {total_bytes}")

    with open(out_path, "wb") as bin:

        # Write initial file size
        bin.write(array('H', [0]).tobytes())

        JUMP_TABLE_TYPE_SIZE = 2  # uint16_t
        JUMP_TABLE_SIZE = len(byte_arrays) * JUMP_TABLE_TYPE_SIZE
        total_size = JUMP_TABLE_SIZE + JUMP_TABLE_TYPE_SIZE

        print(f"Jump Table Size: {total_size}")

        # Write jump table size
        bin.write(array('H', [len(byte_arrays)]).tobytes())

        for i, u8s in enumerate(byte_arrays):
            data_size = len(u8s)
            bin.write(array('H', [total_size]).tobytes())
            total_size += data_size

        for u8s in byte_arrays:
            bin.write(u8s)

        print(f"Total Bytes: {bin.tell()}")

        bin.seek(0)
        bin.write(array('H', [total_size + JUMP_TABLE_TYPE_SIZE]).tobytes())

    print("Done encoding")

    if GENERATE_VIDEO:
        print("Generating Video")
        iio.imwrite(os.path.join(sys.argv[2], f"{filename}.mp4"), [np.array(f.result())
                    for f in process_imgs], fps=175, codec="libx264")
