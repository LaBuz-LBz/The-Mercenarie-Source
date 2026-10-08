# Building the exact build-109 source snapshot

## Prerequisites

- Windows x64.
- Visual C++ **2010 / v100 x64**, with the compatible original toolchain. Newer MSVC runtimes are not an interchangeable replacement for this ABI.
- Windows SDK **7.1**, headers and x64 libraries.
- Python 3 (the preparation used Python 3.12.14).
- Bundled SDK and Boost files, verified by `provenance/dependency-sha256.json`.

Microsoft compilers, SDK binaries, Windows runtime files, the Kenshi executable, game DLLs, and game data are not redistributed here. Obtain them lawfully. No game installation is needed merely to link the plugin using the included import libraries.

## Compile

From this repository in PowerShell, adapting the toolchain paths to your installation:

```powershell
python tools/build.py --vc 'C:/Program Files (x86)/Microsoft Visual Studio 10.0/VC' --sdk 'C:/Program Files/Microsoft SDKs/Windows/v7.1'
```

The script compiles the single translation unit `source/GuildEscort.cpp`, whose includes pull in the implementation headers. Other `.cpp` files are retained tests and development fixtures; compiling all `.cpp` files together is incorrect.

Options preserved from build 109: `/O2 /GL /MD /EHsc /LD /DUNICODE /D_UNICODE`; linker `/LTCG /OPT:REF /OPT:ICF`. `/showIncludes` adds diagnostic output for the audit. The expected explicit import libraries are KenshiLib, OgreMain_x64 and MyGUIEngine_x64. Auto-link directives additionally request Boost.Thread, System, Filesystem, DateTime and Chrono 1.60.0 v100 multithreaded libraries. The original link map shows surviving object code from Thread and System; the other archives still need to be available to the linker.

Outputs: `_build/GuildEscortContracts.dll`, `.map`, `build.log`, `result.json`, and `command.json`. Logs may contain local paths. They are ignored by Git and must not be uploaded with the source snapshot.

For deliberate modifications, use `--allow-modified` after reviewing the changed files. This bypasses hash equality checks, not missing-file checks. The flag does not certify correspondence with build 109.

## Generated files

The exact generated headers used by the DLL are included: `LocalizationDefaults.generated.h`, `CleanupOwnedDefinitions.generated.h`, `RealEstateBaseline.generated.h`, and `SaveBuild.generated.h`. Compilation uses these stored inputs directly and does not silently regenerate them from newer working data. They remain editable text.

The historical generator/input reconstruction is documented separately in `docs/GENERATED_INPUTS.md`. Missing or unverified preferred-source inputs are a publication review issue even if the stored headers compile successfully.

## Installation (not executed during preparation)

1. Obtain the **complete matching build-109 mod resource/data package**. A source DLL alone is insufficient. Public availability of that complete package is unverified.
2. Install the required RE_Kenshi loader and KenshiLib runtime. Reference versions are 0.3.5 and 0.5.0, respectively; their DLLs are not bundled here.
3. Close Kenshi and back up the mod installation and saves yourself.
4. Only after inspecting the result, optionally run:

```powershell
python tools/install.py --mod-dir 'D:/Games/Kenshi/mods/Guild Escort Contracts' --confirm-install
```

This opt-in script refuses a running Kenshi process, saves the previous DLL under `_build/backups/`, and replaces only the plugin DLL. It neither installs resource files nor changes settings or saves. `runtime/RE_Kenshi.json` documents the loader configuration for the existing complete mod installation.

## Reproducibility limits

Hash correspondence of the preserved sources is independently recorded. A rebuilt DLL need not have the same file hash: the original plugin embeds `__DATE__` and `__TIME__`; the PE linker also writes timestamps, and absolute compile paths can affect assert/debug strings. See `provenance/rebuild-comparison.json` for the actual comparison, rather than assuming either equivalence or a code change from hashes alone.

The build script intentionally preserves the original code and generated build ID. A modified release must receive its own identity before distribution. No in-game test was performed during this preparation.

## Verify generated inputs

Run `python tools/check_generators.py`. Three headers regenerate exactly in a temporary directory; the stored source is not modified. The localization input is a newly extracted editable representation, not a claimed historical input.
