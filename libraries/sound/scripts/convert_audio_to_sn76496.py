import argparse
import os
import sys
from typing import List, Tuple
from pathlib import Path

import librosa
import numpy as np
from scipy.signal import butter, lfilter

# PSG constants
SN76496_CLOCK = 3579545
SN76496_DIVISOR = 32
PSG_CLOCK = SN76496_CLOCK / SN76496_DIVISOR
MAX_N = 0x3FF
MAX_WAIT_CYCLES = 15
MAX_DATA_LEN = 4095  # 12-bit limit for RLE indexing

# SN76496 command prefixes
TONE_LATCH = 0x80
TONE_VOL = 0x90

DEFAULT_SAMPLE_HZ = 100
MIN_FREQ = 200
MAX_FREQ = 5000


def sanitize_name(filename: str) -> str:
    """Sanitize filename to be a valid C identifier."""
    return os.path.splitext(os.path.basename(filename))[0].replace('.', '_').replace('-', '_')


def butter_bandpass(lowcut: float, highcut: float, fs: float, order: int = 3) -> Tuple[np.ndarray, np.ndarray]:
    nyq = 0.5 * fs
    low = lowcut / nyq
    high = highcut / nyq
    return butter(order, [low, high], btype='band')


def bandpass_filter(data: np.ndarray, lowcut: float, highcut: float, fs: float, order: int = 3) -> np.ndarray:
    """Applies a bandpass filter to reduce reverb/high frequency noise."""
    b, a = butter_bandpass(lowcut, highcut, fs, order)
    return lfilter(b, a, data)


def hz_to_n(freq: float) -> int:
    """Convert frequency in Hz to SN76496 divider value."""
    if freq <= 0:
        return MAX_N
    return max(1, min(int(PSG_CLOCK / freq), MAX_N))


def amplitude_to_volume(amp: float) -> int:
    """Convert amplitude (0.0-1.0) to SN76496 volume (0-15)."""
    db = 20 * np.log10(amp + 1e-10)
    vol = 15 - int(np.clip((db + 40) / 2.5, 0, 15))
    return vol


def convert_to_psg_commands(path: str, channel: int, sample_rate_hz: float, volume_mult: float) -> Tuple[List[int], List[int]]:
    """
    Convert audio to SN76496 raw PSG command stream and compressed RLE wait table.

    Returns:
        commands: Flat list of uint8_t values to send to port 0xC0
        rle: Encoded list of uint16_t values (index << 4 | count)
    """
    y, sr = librosa.load(path, sr=None, mono=True)
    y = bandpass_filter(y, MIN_FREQ, MAX_FREQ, sr)

    hop = int(sr / sample_rate_hz)
    frames = len(y) // hop

    commands: List[int] = []
    rle: List[int] = []

    prev_freq = -1
    prev_vol = -1
    wait_counter = 0
    last_cmd_index = 0

    for i in range(frames):
        frame = y[i * hop: (i + 1) * hop]
        amp = np.abs(frame).mean()
        vol = amplitude_to_volume(amp * volume_mult)

        try:
            f0 = librosa.yin(frame, fmin=MIN_FREQ, fmax=MAX_FREQ, sr=sr)
            freq = float(np.median(f0)) if len(f0) > 0 else 0.0
        except Exception:
            freq = 0.0

        tone_n = hz_to_n(freq)
        coarse = (tone_n >> 4) & 0x3F
        fine = tone_n & 0x0F

        if tone_n != prev_freq or vol != prev_vol:
            if wait_counter > 0:
                if last_cmd_index >= MAX_DATA_LEN:
                    raise ValueError(f"Command index {last_cmd_index} exceeds 12-bit max of {MAX_DATA_LEN}.")
                if wait_counter > MAX_WAIT_CYCLES:
                    wait_counter = MAX_WAIT_CYCLES
                rle.append((last_cmd_index << 4) | wait_counter)

            wait_counter = 0

            commands.append(TONE_LATCH | (channel << 5) | fine)  # latch + fine
            commands.append(coarse)                              # coarse
            commands.append(TONE_VOL | (channel << 5) | (vol & 0x0F))  # volume

            last_cmd_index = len(commands)
            prev_freq = tone_n
            prev_vol = vol
        else:
            wait_counter += 1

    if wait_counter > 0:
        if last_cmd_index >= MAX_DATA_LEN:
            raise ValueError(f"Final command index {last_cmd_index} exceeds 12-bit max of {MAX_DATA_LEN}.")
        if wait_counter > MAX_WAIT_CYCLES:
            wait_counter = MAX_WAIT_CYCLES
        rle.append((last_cmd_index << 4) | wait_counter)

    return commands, rle


CPP_HEADER_FILE = """// Auto-generated PSG sound data
#pragma once

#include <stddef.h>
#include <stdint.h>

{content}

"""

CPP_NAMESPACE = """
namespace {name} {{

    static const size_t LEN = {length};
    static const size_t RLE_LEN = {rle_length};

    const uint8_t data[LEN] = {{
        // clang-format off
{data}
        // clang-format on
    }};

    const uint16_t {rle_name} = {rle_data};

}} // namespace {name}
"""

CPP_RLE_ARRAY = """{{
        // clang-format off
{data}
        // clang-format on
    }}"""


def tab_text(string: str, count: int = 1) -> str:
    for i in range(count):
        string = '    ' + '    '.join(string.splitlines(True))
    return string


def write_cpp_file(name: str, output_dir: Path, psg_data: List[int], rle_data: List[int]) -> None:
    """Writes out a C file with the PSG command stream and wait table."""
    filename = f"{name}.hpp"

    length = len(psg_data)
    rle_length = len(rle_data)

    data_str = ""
    rle_data_str = ""

    for i, val in enumerate(psg_data):
        if i % 16 == 0:
            data_str += "    "
        data_str += f"0x{val:02X}, "
        if (i + 1) % 16 == 0 and i != length - 1:
            data_str += "\n"

    for i, val in enumerate(rle_data):
        if i % 8 == 0:
            rle_data_str += "    "
        rle_data_str += f"0x{val:04X}, "
        if (i + 1) % 8 == 0 and i != rle_length - 1:
            rle_data_str += "\n"

    data_str = tab_text(data_str)
    rle_data_str = tab_text(rle_data_str)

    if rle_length > 0:
        rle_data_str = CPP_RLE_ARRAY.format(data=rle_data_str)
        rle_name = "rle[RLE_LEN]"
    else:
        rle_data_str = "nullptr"
        rle_name = "*rle"

    namespace_str = CPP_NAMESPACE.format(name=name.title(), length=length,
                                         rle_length=rle_length, data=data_str, rle_name=rle_name, rle_data=rle_data_str)

    with open(output_dir / filename, "w", encoding="utf-8") as f:
        f.write(CPP_HEADER_FILE.format(content=namespace_str))

    print(f"[OK] Wrote: {filename}")


def main() -> None:
    """Main function using argparse for input file and channel."""
    parser = argparse.ArgumentParser(description="Convert audio file to SN76496 PSG command stream.")
    parser.add_argument("input", help="Input audio file (wav, mp3, etc.)")
    parser.add_argument("channel", type=int, default=0, choices=[0, 1, 2], help="PSG channel (0, 1, or 2)")
    parser.add_argument("-o", "--output", type=Path, default=Path("."), help="Output directory")
    parser.add_argument("-s", "--sample_rate", type=float, default=DEFAULT_SAMPLE_HZ,
                        help="Rate in Hz to sample the audio file")
    parser.add_argument("-v", "--volume", type=int, default=100, help="Volume multiplier out of 100")

    args = parser.parse_args()
    input_file = args.input
    channel = args.channel
    output_directory = args.output
    sample_rate = args.sample_rate
    volume = args.volume / 100.0

    name = sanitize_name(input_file)
    try:
        psg_data, rle_data = convert_to_psg_commands(input_file, channel, sample_rate, volume)
        if len(psg_data) > MAX_DATA_LEN:
            raise ValueError(f"PSG data exceeds {MAX_DATA_LEN} bytes. Try trimming the audio.")
        write_cpp_file(name, output_directory, psg_data, rle_data)
    except Exception as e:
        print(f"[ERROR] {e}")
        sys.exit(1)


if __name__ == "__main__":
    main()
