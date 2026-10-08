# Exact runtime source investigation

The prior release-file comparison identifies RE_Kenshi 0.3.5 and KenshiLib 0.5.0. The compilation SDK remains the exact mixed snapshot, including a KenshiLib 0.4.0 import library and five local header adaptations. Those files and the existing patch/provenance manifests are unchanged.

`provenance/runtime-source-lock.json` records the actual commits obtained through a recursive checkout of the official RE_Kenshi v0.3.5 tag. KenshiLib's submodule commit is also its v0.5.0 tag. CompressTools, RapidJSON and its nested GoogleTest are pinned; no local modifications were found in these fetched trees. Complete tracked upstream files including notices have been prepared in a PRIVATE review archive, not silently added to the public candidate. GoogleTest is a test dependency, not evidence of code linked into the mod.

Remaining exact-build inputs are NOT pinned by those upstream trees:
- KenshiLib README names MinHook's `multihook` branch; `KenshiLib.vcxproj:53-54,84` uses external MINHOOK_PATH and libMinHook.x64.lib. A current branch checkout is not evidence of the version used for the distributed binary.
- CompressToolsCore.vcxproj:40-46,81 expects external ogre-next-deps include/lib paths and FreeImage.lib. That tree/version and its component licenses/source are not resolved by recursive checkout.
- RE_Kenshi uses external Microsoft/DirectX toolchains; do not redistribute proprietary SDKs. Its runtime build has not been reproduced here.

Ask the upstream runtime maintainer for the release build dependency revisions, any local patches, and matching source delivery for MinHook/FreeImage/ogre-next-deps where applicable. No message has been sent. The legal scope of corresponding source for separately obtained runtime components must be determined from actual distribution/combined-work circumstances; no blanket claim that every runtime library must always be bundled here is made.

References: https://github.com/BFrizzleFoShizzle/RE_Kenshi/tree/v0.3.5 ; https://github.com/BFrizzleFoShizzle/KenshiLib/tree/v0.5.0 ; GNU GPLv3 section 1 and section 6: https://www.gnu.org/licenses/gpl.en.html
