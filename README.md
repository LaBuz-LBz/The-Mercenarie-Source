# The-Mercenarie-Source
Source code for The Mercenarie, a Kenshi mod using RE_Kenshi and KenshiLib.

---

# The Mercenarie — source preparation

The Mercenarie is a guild and contracts mod for **Kenshi**, developed by **LaBuz-LBz**. Its native plugin uses **RE_Kenshi** as its loader and **KenshiLib** to access and hook game functionality.

## Exact version and current status

This snapshot corresponds to **V9-CONTRACT-BOARD-20261008-109**. On 8 October 2026 the project owner explicitly confirmed that this is the V9 distributed to players. This is an owner confirmation, not an independent download check of Steam Workshop. Planned tag: `v9-dev.109`.

Original DLL SHA-256:

`3f5e35aca29d8685bf2e583570ed50d1ad0518d87115302ce6c978a2c84df423`

All 395 files in `source/` are byte-for-byte copies of the recorded build-109 source snapshot. See `provenance/` for file hashes and dependency provenance. New build and installation scripts were prepared separately; original plugin source was not edited.

**This is a review candidate, not a certification of complete GPL compliance.** See [publication blockers](docs/PUBLICATION_REVIEW.md), [build instructions](BUILDING.md), and [third-party notices](THIRD_PARTY_NOTICES.md). No release or Git tag has been created by this preparation.

## Download and requirements

- Mod distribution page: [The Mercenarie on Steam Workshop](https://steamcommunity.com/sharedfiles/filedetails/?id=3798671709). The exact build currently served there has not been checked against this snapshot.
- Loader: [RE_Kenshi](https://github.com/BFrizzleFoShizzle/RE_Kenshi), locally verified runtime version **0.3.5**.
- Native API: [KenshiLib](https://github.com/BFrizzleFoShizzle/KenshiLib), locally verified runtime version **0.5.0**.
- A lawful installation of Kenshi and a complete matching mod data/resource package are required to run the plugin. This source repository is **not** a standalone replacement for the complete mod download.

The compilation SDK is a preserved, mixed snapshot, not a clean KenshiLib 0.5.0 SDK. Its KenshiLib import library matches upstream 0.4.0. Do not upgrade dependencies when checking correspondence with this binary.

## Building

Install the Visual C++ 2010 x64 toolchain and Windows SDK 7.1, plus Python 3. Run `tools/build.py` with their directories as explained in [BUILDING.md](BUILDING.md). The script verifies the recorded inputs and writes only to `_build/`. It does not launch Kenshi or install the result.

## Licensing and credits

The project owner requested preparation for a GPLv3 source release. `LICENSE` preserves the GNU GPL version 3 text already present in the destination repository. The proposed project-code release scope is `source/`, `compat/`, and the newly supplied project tools; third-party-derived portions retain their upstream notices and obligations. Publication and final scope approval remain pending. No additional “or later” grant or linking exception is inferred for dependencies.

Credit belongs to LaBuz-LBz for The Mercenarie, BFrizzleFoShizzle and KenshiLib contributors for RE_Kenshi/KenshiLib, Boost contributors, OGRE/Torus Knot Software contributors, MyGUI Developers, and OIS/Phillip Castaneda and contributors. Kenshi belongs to Lo-Fi Games. This repository does not grant rights to Kenshi or to separately licensed mod artwork, models, fonts, or other assets.

Upstream copyright and license texts are preserved in their original files and `licenses/`. The local SDK adjustments and the SDK-derived `source/v5/NavMeshCompat.h` are documented in [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).

## Future versions

Use one repository with ordinary commit history, not a new repository for every release. A proposed identifier for this snapshot is `v9-dev.109`; it must not be renamed `v9.0` without establishing that it is the actual distributed release. Future V9.1, V10 and V11 tags should point to the exact source commits used for their distributed binaries, with an updated binary/source/dependency manifest. See [future automation proposal](docs/FUTURE_RELEASES.md).
