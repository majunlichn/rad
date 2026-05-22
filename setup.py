#!/usr/bin/env python3
"""Clone, build, and install third-party libraries under external/."""

from __future__ import annotations

import argparse
import os
import shlex
import shutil
import subprocess
import sys
from dataclasses import dataclass, field
from pathlib import Path

ROOT = Path(__file__).resolve().parent
EXTERNAL = ROOT / "external"
BUILD_TYPES = ("Release", "Debug", "RelWithDebInfo", "MinSizeRel")
DEFAULT_LIBS = ("sdl3", "sdl3-mixer")


def cpu_count() -> int:
    return os.cpu_count() or 1


@dataclass
class BuildContext:
    generator: str | None = None
    architecture: str | None = None
    build_type: str = "Release"
    jobs: int = 1
    force: bool = False


@dataclass(frozen=True)
class ThirdPartyLib:
    name: str
    source_dir: Path
    build_dir: Path
    install_dir: Path
    repo_url: str
    git_tag: str
    cmake_configure_options: list[str] = field(default_factory=list)
    git_recursive: bool = False
    cmake_deps: tuple[str, ...] = ()
    cmake_package_configs: tuple[str, ...] = ()

    @property
    def is_package_installed(self) -> bool:
        if not self.cmake_package_configs:
            return self.install_dir.is_dir() and any(self.install_dir.iterdir())
        return any((self.install_dir / rel).is_file() for rel in self.cmake_package_configs)


LIBRARIES: dict[str, ThirdPartyLib] = {
    "sdl3": ThirdPartyLib(
        name="SDL3",
        source_dir=EXTERNAL / "SDL" / "source",
        build_dir=EXTERNAL / "SDL" / "build",
        install_dir=EXTERNAL / "SDL" / "installed",
        repo_url="https://github.com/libsdl-org/SDL.git",
        git_tag="release-3.4.8",
        cmake_configure_options=[
            "-DSDL_TEST_LIBRARY=OFF",
            "-DSDL_TESTS=OFF",
            "-DSDL_EXAMPLES=OFF",
        ],
        cmake_package_configs=(
            "cmake/SDL3Config.cmake",
            "lib/cmake/SDL3/SDL3Config.cmake",
        ),
    ),
    "sdl3-mixer": ThirdPartyLib(
        name="SDL3_mixer",
        source_dir=EXTERNAL / "SDL_mixer" / "source",
        build_dir=EXTERNAL / "SDL_mixer" / "build",
        install_dir=EXTERNAL / "SDL_mixer" / "installed",
        repo_url="https://github.com/libsdl-org/SDL_mixer.git",
        git_tag="release-3.2.2",
        git_recursive=True,
        cmake_deps=("sdl3",),
        cmake_configure_options=[
            "-DSDLMIXER_TESTS=OFF",
            "-DSDLMIXER_EXAMPLES=OFF",
        ],
        cmake_package_configs=(
            "cmake/SDL3_mixerConfig.cmake",
            "lib/cmake/SDL3_mixer/SDL3_mixerConfig.cmake",
        ),
    ),
}

EPILOG = """\
examples:
  python setup.py
  python setup.py --only sdl3
  python setup.py --only sdl3 --force

Dependencies are built automatically (e.g. sdl3 for sdl3-mixer).
--force clears build/ before configure, rebuilds, then clears install/ before reinstalling (keeps source/).
"""


def parse_targets(only: list[str] | None) -> list[str]:
    if not only:
        return list(DEFAULT_LIBS)
    unknown = set(only) - set(LIBRARIES)
    if unknown:
        available = ", ".join(sorted(LIBRARIES))
        raise RuntimeError(f"Unknown libraries: {', '.join(sorted(unknown))}. Available: {available}")
    return list(only)


def resolve_dependencies(keys: list[str]) -> list[str]:
    resolved: list[str] = []
    visited: set[str] = set()

    def add_with_deps(key: str) -> None:
        if key in visited:
            return
        visited.add(key)
        for dep in LIBRARIES[key].cmake_deps:
            add_with_deps(dep)
        resolved.append(key)

    for key in keys:
        add_with_deps(key)
    return resolved


def find_cmake_package_dir(lib: ThirdPartyLib) -> Path | None:
    for rel in lib.cmake_package_configs:
        config_file = lib.install_dir / rel
        if config_file.is_file():
            return config_file.parent
    return None


def get_cmake_dep_args(lib: ThirdPartyLib) -> list[str]:
    args: list[str] = []
    for dep_key in lib.cmake_deps:
        dep = LIBRARIES[dep_key]
        if not dep.is_package_installed:
            raise RuntimeError(f"{lib.name} requires {dep.name} ({dep_key}); build it first")
        dep_dir = find_cmake_package_dir(dep)
        if dep_dir is None:
            raise RuntimeError(
                f"{lib.name} requires {dep.name} ({dep_key}); "
                f"no CMake package under {dep.install_dir}"
            )
        args.append(f"-D{dep.name}_DIR={dep_dir}")
    return args


def format_cmd(cmd: list[str]) -> str:
    """Shell-style command line for logs (subprocess still uses a argv list)."""
    if os.name == "nt":
        return subprocess.list2cmdline(cmd)
    return shlex.join(cmd)


def run(cmd: list[str], *, cwd: Path | None = None) -> int:
    print(f"+ {format_cmd(cmd)}", flush=True)
    return subprocess.run(cmd, cwd=cwd).returncode


def run_quiet(cmd: list[str], *, cwd: Path | None = None) -> int:
    return subprocess.run(cmd, cwd=cwd, capture_output=True, text=True).returncode


def run_checked(cmd: list[str], *, cwd: Path | None = None) -> None:
    code = run(cmd, cwd=cwd)
    if code:
        raise subprocess.CalledProcessError(code, cmd)


def log(msg: str) -> None:
    print(msg, flush=True)


def git_rev_parse(repo: Path, ref: str) -> str | None:
    result = subprocess.run(
        ["git", "-C", str(repo), "rev-parse", ref],
        capture_output=True,
        text=True,
    )
    if result.returncode != 0:
        return None
    return result.stdout.strip()


def resolve_tag_commit_local(repo: Path, tag: str) -> str | None:
    for ref in (
        f"refs/tags/{tag}^{{commit}}",
        f"refs/tags/{tag}",
        tag,
        f"origin/{tag}",
        f"refs/remotes/origin/{tag}",
    ):
        commit = git_rev_parse(repo, ref)
        if commit:
            return commit
    return None


def git_fetch_tag(repo: Path, tag: str) -> bool:
    log(f"  source: fetching {tag} from origin")
    fetch_specs = (
        f"+refs/tags/{tag}:refs/tags/{tag}",
        f"refs/tags/{tag}:refs/tags/{tag}",
    )
    for spec in fetch_specs:
        if run_quiet(["git", "-C", str(repo), "fetch", "--depth", "1", "--force", "origin", spec]) == 0:
            return True
    for extra in (("tag", tag), (tag,)):
        if run_quiet(["git", "-C", str(repo), "fetch", "--depth", "1", "origin", *extra]) == 0:
            return True
    return False


def resolve_tag_commit(repo: Path, tag: str) -> str:
    commit = resolve_tag_commit_local(repo, tag)
    if commit:
        return commit
    if not git_fetch_tag(repo, tag):
        raise RuntimeError(f"Could not fetch tag '{tag}' from origin in {repo}")
    commit = resolve_tag_commit_local(repo, tag)
    if commit:
        return commit
    # Only trust FETCH_HEAD right after we fetched this tag (not a stale prior fetch).
    commit = git_rev_parse(repo, "FETCH_HEAD")
    if commit:
        return commit
    raise RuntimeError(f"Could not resolve git ref '{tag}' in {repo}")


def ensure_source_tag(repo: Path, tag: str, *, recursive: bool = False) -> None:
    if not (repo / ".git").exists():
        raise RuntimeError(f"Source at {repo} exists but is not a git repository")

    target = resolve_tag_commit(repo, tag)
    head = git_rev_parse(repo, "HEAD")
    if head != target:
        log(f"  source: checking out {tag}")
        if run(["git", "-C", str(repo), "checkout", "--force", target]) != 0:
            run_checked(["git", "-C", str(repo), "fetch", "--depth", "1", "origin", target])
            run_checked(["git", "-C", str(repo), "checkout", "--force", target])
    else:
        log(f"  source: already at {tag}")

    if recursive:
        run_checked(
            [
                "git",
                "-C",
                str(repo),
                "submodule",
                "update",
                "--init",
                "--recursive",
                "--depth",
                "1",
            ]
        )


def git_clone(url: str, dest: Path, tag: str, *, recursive: bool = False) -> None:
    if (dest / "CMakeLists.txt").is_file():
        ensure_source_tag(dest, tag, recursive=recursive)
        return
    if dest.exists():
        raise RuntimeError(f"Incomplete source tree at {dest}; remove it under external/")

    log(f"  source: cloning {tag} into {dest}")
    dest.parent.mkdir(parents=True, exist_ok=True)
    cmd = ["git", "clone", "--depth", "1", "--branch", tag, url, dest.name]
    if recursive:
        cmd.insert(-1, "--recurse-submodules")
    run_checked(cmd, cwd=dest.parent)


def cmake_configure(
    lib: ThirdPartyLib, ctx: BuildContext, extra_args: list[str]
) -> None:
    lib.build_dir.mkdir(parents=True, exist_ok=True)
    args = [
        "cmake",
        "-S",
        str(lib.source_dir),
        "-B",
        str(lib.build_dir),
        f"-DCMAKE_INSTALL_PREFIX={lib.install_dir}",
    ]
    gen = (ctx.generator or "").casefold()
    if "visual studio" not in gen and "xcode" not in gen:
        args.append(f"-DCMAKE_BUILD_TYPE={ctx.build_type}")
    if ctx.generator:
        args.extend(["-G", ctx.generator])
    if ctx.architecture:
        args.extend(["-A", ctx.architecture])
    args.extend(extra_args)
    run_checked(args)


def cmake_build(lib: ThirdPartyLib, ctx: BuildContext) -> None:
    args = [
        "cmake",
        "--build",
        str(lib.build_dir),
        "--config",
        ctx.build_type,
    ]
    args.extend(["--parallel", str(ctx.jobs)])
    run_checked(args, cwd=ROOT)


def cmake_install(lib: ThirdPartyLib, ctx: BuildContext) -> None:
    args = [
        "cmake",
        "--build",
        str(lib.build_dir),
        "--target",
        "install",
        "--config",
        ctx.build_type,
    ]
    args.extend(["--parallel", str(ctx.jobs)])
    run_checked(args, cwd=ROOT)


def setup_library(ctx: BuildContext, key: str, *, force: bool | None = None) -> None:
    force_rebuild = ctx.force if force is None else force
    lib = LIBRARIES[key]
    label = f"{key} ({lib.name})"
    if lib.is_package_installed and not force_rebuild:
        log(f"{label}: already installed at {lib.install_dir}")
        return

    log(f"=== {label} ===")
    git_clone(lib.repo_url, lib.source_dir, lib.git_tag, recursive=lib.git_recursive)
    if force_rebuild and lib.build_dir.exists():
        log(f"  force: removing {lib.build_dir}")
        shutil.rmtree(lib.build_dir)

    configure_args = get_cmake_dep_args(lib) + list(lib.cmake_configure_options)
    cmake_configure(lib, ctx, configure_args)
    if force_rebuild:
        cmake_build(lib, ctx)
        if lib.install_dir.exists():
            log(f"  force: removing {lib.install_dir}")
            shutil.rmtree(lib.install_dir)
    cmake_install(lib, ctx)

    if not lib.is_package_installed:
        expected = ", ".join(lib.cmake_package_configs) or "(any file)"
        raise RuntimeError(
            f"{lib.name} install missing CMake package config under {lib.install_dir} "
            f"(expected one of: {expected})"
        )
    log(f"  installed to {lib.install_dir}")


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Build third-party libraries under external/.",
        epilog=EPILOG,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument(
        "--only",
        nargs="+",
        choices=sorted(LIBRARIES),
        metavar="LIB",
        help=f"Libraries to build (default: {' '.join(DEFAULT_LIBS)})",
    )
    parser.add_argument("--force", action="store_true", help="Rebuild listed libraries")
    parser.add_argument("--build-type", choices=BUILD_TYPES, default="Release")
    cpus = cpu_count()
    parser.add_argument(
        "-j",
        "--jobs",
        type=int,
        default=None,
        metavar="N",
        help=f"Parallel build jobs (default: {cpus}, max: {cpus})",
    )
    parser.add_argument("-G", "--generator", metavar="GENERATOR")
    parser.add_argument("-A", "--architecture", metavar="ARCH")
    args = parser.parse_args()

    if args.jobs is not None:
        if args.jobs < 1:
            print("error: --jobs must be at least 1", file=sys.stderr)
            return 1
        if args.jobs > cpus:
            print(f"error: --jobs must be at most {cpus}", file=sys.stderr)
            return 1
    jobs = args.jobs if args.jobs is not None else cpus
    if shutil.which("git") is None:
        print("error: git not found on PATH", file=sys.stderr)
        return 1
    if shutil.which("cmake") is None:
        print("error: cmake not found on PATH", file=sys.stderr)
        return 1

    try:
        requested = parse_targets(args.only)
        targets = resolve_dependencies(requested)
        generator = (args.generator or "").strip() or None
        if not generator:
            env_gen = os.environ.get("CMAKE_GENERATOR", "").strip()
            if env_gen:
                generator = env_gen
                log(f"CMAKE_GENERATOR={generator}")
        architecture = (args.architecture or "").strip() or None
        if not architecture:
            env_arch = os.environ.get("CMAKE_GENERATOR_PLATFORM", "").strip()
            if env_arch:
                architecture = env_arch
                log(f"CMAKE_GENERATOR_PLATFORM={architecture}")
        if architecture and not generator:
            print("note: -A ignored without -G (requires a Visual Studio generator)", file=sys.stderr)
            architecture = None

        ctx = BuildContext(
            generator=generator,
            architecture=architecture,
            build_type=args.build_type,
            jobs=jobs,
            force=args.force,
        )

        build_line = ", ".join(targets)
        if requested != targets:
            build_line += f" (from {', '.join(requested)})"
        if ctx.force:
            build_line += " (force)"
        log(f"Building: {build_line}")
        if generator:
            arch = f" -A {architecture}" if architecture else ""
            log(f"CMake: -G {generator}{arch}, --config {ctx.build_type}")
        else:
            log(f"CMake: default generator, build type {ctx.build_type}")
        log(f"Parallel jobs: {ctx.jobs}")
        log("")

        EXTERNAL.mkdir(parents=True, exist_ok=True)
        requested_set = set(requested)
        for key in targets:
            setup_library(ctx, key, force=ctx.force and key in requested_set)

        hints: list[str] = []
        for key in targets:
            lib = LIBRARIES[key]
            config_dir = find_cmake_package_dir(lib)
            if config_dir:
                hints.append(f"  -D{lib.name}_DIR={config_dir}")
        log("")
        if hints:
            log("Done. CMake package dirs (if needed):")
            for line in hints:
                log(line)
        else:
            log("Done.")
    except subprocess.CalledProcessError as error:
        cmd = error.cmd
        cmd_str = " ".join(cmd) if isinstance(cmd, list) else str(cmd)
        print(f"error: command failed ({cmd_str})", file=sys.stderr)
        return error.returncode or 1
    except RuntimeError as error:
        print(f"error: {error}", file=sys.stderr)
        return 1

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
