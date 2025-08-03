#!/usr/bin/env python3
import argparse
import os
import sys
from typing import List, Tuple
from pathlib import Path

import librosa
import numpy as np
from scipy.signal import butter, lfilter

# Constants
SN76496_CLOCK = 3579545
SN76496_DIVISOR = 32
PSG_CLOCK = SN76496_CLOCK / SN76496_DIVISOR
MAX_N = 0x3FF
MAX_WAIT_CYCLES = 15
MAX_DATA_LEN = 4095  # 12-bit limit for RLE indexing

# PSG command base
NOISE_CMD = 0xE0
NOISE_VOL = 0xF0
TONE_LATCH = 0x80
TONE_VOL = 0x90

DEFAULT_SAMPLE_HZ = 100
MIN_FREQ = 80
MAX_FREQ = 3000


def sanitize_name(filename: str) -> str:
    return os.path.splitext(os.path.basename(filename))[0].replace('.', '_').replace('-', '_')


def butter_bandpass(lowcut: float, highcut: float, fs: float, order: int = 3):
    nyq = 0.5 * fs
    return butter(order, [lowcut / nyq, highcut / nyq], btype='band')


def bandpass_filter(data: np.ndarray, lowcut: float, highcut: float, fs: float, order: int = 3):
    b, a = butter_bandpass(lowcut, highcut, fs, order)
    return lfilter(b, a, data)


def compress_spectrum(y: np.ndarray, sr: int, min_target: float = 80.0, max_target: float = 3000.0) -> np.ndarray:
    centroid = librosa.feature.spectral_centroid(y=y, sr=sr)[0]
    avg_centroid = np.mean(centroid)
    desired_center = (min_target + max_target) / 2.0
    semitone_shift = 12 * np.log2(desired_center / avg_centroid)
    print(f"[INFO] Pitch shifting by {semitone_shift:.2f} semitones to center spectral content.")
    return librosa.effects.pitch_shift(y, sr=sr, n_steps=-semitone_shift)


def amplitude_to_volume(amp: float) -> int:
    db = 20 * np.log10(amp + 1e-10)
    vol = 15 - int(np.clip((db + 40) / 2.5, 0, 15))
    return vol


def hz_to_n(freq: float) -> int:
    return max(1, min(int(PSG_CLOCK / freq), MAX_N))


def convert_to_psg_noise(path: str, sample_rate_hz: float, volume_mult: float,
                         tie_channel2_freq: bool = False) -> Tuple[List[int], List[int]]:
    y, sr = librosa.load(path, sr=None, mono=True)

    y = compress_spectrum(y, sr, min_target=MIN_FREQ, max_target=MAX_FREQ)
    y = bandpass_filter(y, MIN_FREQ, MAX_FREQ, sr)

    hop = int(sr / sample_rate_hz)
    frames = len(y) // hop

    commands: List[int] = []
    rle: List[int] = []

    prev_vol = -1
    prev_freq_n = -1
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

        # Encode PSG noise settings (use bits 0-2 for mode + period)
        noise_type = (tone_n & 0x07)

        command_changed = False
        cmd_start_idx = len(commands)

        # If tied to channel 2, update tone latch + coarse
        if tie_channel2_freq and tone_n != prev_freq_n:
            commands.append(TONE_LATCH | (2 << 5) | fine)
            commands.append(coarse)
            prev_freq_n = tone_n
            command_changed = True

        if vol != prev_vol or command_changed:
            commands.append(NOISE_CMD | noise_type)
            commands.append(NOISE_VOL | (vol & 0x0F))
            prev_vol = vol
            command_changed = True

        if command_changed:
            if wait_counter > 0:
                if last_cmd_index >= MAX_DATA_LEN:
                    raise ValueError(f"RLE index {last_cmd_index} exceeds max {MAX_DATA_LEN}")
                rle.append((last_cmd_index << 4) | min(wait_counter, MAX_WAIT_CYCLES))
            last_cmd_index = cmd_start_idx
            wait_counter = 0
        else:
            wait_counter += 1

    if wait_counter > 0:
        rle.append((last_cmd_index << 4) | min(wait_counter, MAX_WAIT_CYCLES))

    return commands, rle


def tab_text(string: str, count: int = 1) -> str:
    for i in range(count):
        string = '    ' + '    '.join(string.splitlines(True))
    return string


def write_cpp_file(name: str, output_dir: Path, psg_data: List[int], rle_data: List[int]) -> None:
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
        rle_block = f"""{{
        // clang-format off
{rle_data_str}
        // clang-format on
    }}"""
        rle_decl = "rle[RLE_LEN]"
    else:
        rle_block = "nullptr"
        rle_decl = "*rle"

    cpp_header = f"""// Auto-generated PSG sound data
#pragma once

#include <stddef.h>
#include <stdint.h>

namespace {name.title()} {{

    static const size_t LEN = {length};
    static const size_t RLE_LEN = {rle_length};

    const uint8_t data[LEN] = {{
        // clang-format off
{data_str}
        // clang-format on
    }};

    const uint16_t {rle_decl} = {rle_block};

}} // namespace {name.title()}
"""
    with open(output_dir / filename, "w", encoding="utf-8") as f:
        f.write(cpp_header)

    print(f"[OK] Wrote: {filename}")


def main() -> None:
    parser = argparse.ArgumentParser(description="Convert audio file to SN76496 noise PSG stream")
    parser.add_argument("input", help="Input audio file (wav, mp3, etc.)")
    parser.add_argument("-o", "--output", type=Path, default=Path("."), help="Output directory")
    parser.add_argument("-s", "--sample_rate", type=float, default=DEFAULT_SAMPLE_HZ, help="Sampling rate in Hz")
    parser.add_argument("-v", "--volume", type=int, default=100, help="Volume multiplier (0-100)")
    parser.add_argument("--tie-channel-2", action="store_true", help="Tie noise frequency to channel 2 tone")

    args = parser.parse_args()
    input_file = args.input
    output_directory = args.output
    sample_rate = args.sample_rate
    volume = args.volume / 100.0
    tie_channel2 = args.tie_channel_2

    name = sanitize_name(input_file)
    try:
        psg_data, rle_data = convert_to_psg_noise(input_file, sample_rate, volume, tie_channel2)
        if len(psg_data) > MAX_DATA_LEN:
            raise ValueError(f"PSG data exceeds {MAX_DATA_LEN} bytes.")
        write_cpp_file(name, output_directory, psg_data, rle_data)
    except Exception as e:
        print(f"[ERROR] {e}")
        sys.exit(1)


if __name__ == "__main__":
    main()
