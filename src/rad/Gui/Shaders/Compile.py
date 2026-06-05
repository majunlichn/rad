#!/usr/bin/env python3
"""Compile GUI GLSL shaders to SPIR-V."""

from __future__ import annotations

import argparse
import subprocess
import sys
from pathlib import Path

SHADER_DIR = Path(__file__).resolve().parent
GLSLC = Path()
OUTPUT_DIR = Path()


def run_glslc(*args: object) -> None:
    command = [str(GLSLC), *(str(arg) for arg in args)]
    print(f"Compile.py: {' '.join(command)}")
    subprocess.run(command, check=True)


def compile_shaders() -> None:
    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)

    run_glslc("--target-env=vulkan1.3", SHADER_DIR / "GuiComposition.vert", "-o", OUTPUT_DIR / "GuiComposition.vert.spv")
    run_glslc("--target-env=vulkan1.3", SHADER_DIR / "GuiComposition.frag", "-o", OUTPUT_DIR / "GuiComposition.frag.spv")


def main() -> int:
    parser = argparse.ArgumentParser(description="Compile GUI GLSL shaders to SPIR-V.")
    parser.add_argument("--glslc", required=True, type=Path, help="Path to the glslc executable.")
    parser.add_argument("--output-dir", required=True, type=Path, help="Output directory for .spv files.")
    args = parser.parse_args()

    global GLSLC, OUTPUT_DIR
    GLSLC = args.glslc.resolve()
    OUTPUT_DIR = args.output_dir.resolve()

    if not GLSLC.is_file():
        print(f"Compile.py: glslc not found: {GLSLC}", file=sys.stderr)
        return 1

    try:
        compile_shaders()
    except subprocess.CalledProcessError as error:
        print(f"Compile.py: glslc failed with exit code {error.returncode}", file=sys.stderr)
        return error.returncode or 1

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
