# Third-party notices and provenance

These notices distinguish the plugin's compilation inputs from independently installed runtime software. A technical inventory is not a definitive legal opinion.

| Component | Version / identity | License evidence | Use / inclusion |
|---|---|---|---|
| The Mercenarie | V9-CONTRACT-BOARD-20261008-109 | Proposed GPLv3 release; root LICENSE copied unchanged from the owner's public repository | Exact project source; publication pending |
| KenshiLib SDK | Preserved mixed snapshot, import library identical to 0.4.0 | GPLv3 text in upstream 0.4.0/0.5.0 repository; `licenses/KenshiLib-LICENSE.txt`; nested licenses retained | Headers and import library included; local changes recorded |
| KenshiLib runtime | 0.5.0, exact upstream release binary match locally | Upstream GPLv3 plus bundled third-party notices | Dynamic runtime dependency; DLL not redistributed here |
| RE_Kenshi | 0.3.5, exact upstream loose-release binary match locally | `licenses/RE_Kenshi-LICENSE.txt` | Plugin loader, not a direct DLL import; not redistributed here |
| Boost | 1.60.0 | `licenses/Boost-BSL-1.0.txt`, per-file notices | Headers, five required static link archives, and corresponding Thread/System/Filesystem/DateTime/Chrono source directories included |
| OGRE SDK | Header macros: 2.0.0 unstable / Tindalos | MIT, `licenses/OGRE-MIT.txt`, per-file copyright | Headers and import library included; game runtime binary not included. Header version is not proof of exact game DLL revision |
| MyGUI SDK | Header macros: 3.2.3 | MIT, `licenses/MyGUI-MIT.txt` | Headers and import library included; two local diagnostic-path adjustments preserved; game DLL not included |
| OIS SDK | Header macros: 1.2.0 / Smash | zlib/libpng-style license, `licenses/OIS-zlib.txt` | Headers through SDK; no OIS DLL supplied |
| Microsoft VC100 / Windows SDK | v100 x64 / Windows SDK 7.1 | Proprietary toolchain/system licenses | External prerequisites, not redistributed |
| Kenshi / game middleware | Game-specific interfaces and resource dependencies | No redistribution grant inferred for the game or its middleware | Game binaries, assets and original proprietary implementations excluded |

## SDK origin and changes

847 of 855 local SDK files match Git blobs in [KenshiLib_Examples_deps commit b566d74](https://github.com/BFrizzleFoShizzle/KenshiLib_Examples_deps/tree/b566d74bf3d74629cc2fb632a97595b8202993f1). All three import libraries additionally match the SHA-256 object IDs in that repository's Git LFS pointers. The remaining five header differences are local compatibility adjustments. `provenance/sdk-provenance.json` records the file-by-file results; `sdk-local-patches.diff` compares the local SDK against KenshiLib 0.4.0, also showing two headers updated relative to that older tag. Do not describe the entire SDK as pristine 0.4.0 or 0.5.0.

`source/v5/NavMeshCompat.h` explicitly identifies itself as a local SDK compatibility copy with typename corrections; it is not exclusively original Mercenarie code. Preserve the KenshiLib attribution and GPL scope for this derived header. The Kenshi interface declarations originate from the public reconstruction project; inclusion is not a claim to ownership of Kenshi's implementation.

The repository's local modifications to third-party headers are preserved exactly rather than overwritten with newer upstream files. Changes and dates of this packaging preparation are recorded here (8 October 2026); historical author/date of each earlier adaptation is not independently established.

## Sources checked

- [KenshiLib 0.5.0](https://github.com/BFrizzleFoShizzle/KenshiLib/tree/v0.5.0), [0.4.0](https://github.com/BFrizzleFoShizzle/KenshiLib/tree/v0.4.0).
- [RE_Kenshi 0.3.5](https://github.com/BFrizzleFoShizzle/RE_Kenshi/tree/v0.3.5).
- [Boost 1.60.0 source archive](https://archives.boost.io/release/1.60.0/source/boost_1_60_0.tar.bz2), SHA-256 `686affff989ac2488f79a97b9479efb9f2abae035b5ed4d8226de6857933fd3b`.
- [GNU GPL v3](https://www.gnu.org/licenses/gpl-3.0.html), [GNU FAQ on corresponding source](https://www.gnu.org/licenses/gpl-faq.html#CompleteCorrespondingSource), [linking](https://www.gnu.org/licenses/gpl-faq.html#GPLStaticVsDynamic).

Full runtime library source and every transitive runtime dependency are not bundled in this candidate. Links alone do not establish that every applicable corresponding-source delivery obligation has been met. RE_Kenshi's source archive contains submodule references; the top-level archive alone is not its full recursive build tree. Decide the required delivery scope and preserve the exact submodule revisions before publishing a compliance claim.

## Assets and other materials

The root GPL text is not a blanket relicensing of Kenshi, mod models, textures, fonts, or third-party furniture. The existing mod notice records permission to integrate Mechanica's Random Furniture chair assets; it does not establish a general GPL relicensing or permission to publish editable asset sources. Those assets are not copied into this repository. Translation JSON files are included as runtime text inputs, with provenance retained; confirm contributor permissions before publication.

`RealEstateBaseline.generated.h` contains facts/IDs generated from game base data. Its exact bytes are preserved because they are a build input. Its preferred generation inputs and any applicable database/IP issues require review; no original game database is redistributed here. See `docs/PUBLICATION_REVIEW.md`.
