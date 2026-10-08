Follow-up: GENERATED_INPUTS.md, RESOURCE_SCOPE.md and RUNTIME_SOURCES.md supersede the earlier technical uncertainties. Remaining legal and upstream dependency questions still prevent a complete compliance claim.

# Publication review — local commit authorized, push pending

## Technically established

- Owner selected the latest local build, **109**, after being told that several older V9 and beta archives exist.
- Original DLL and installed DLL match; 395 copied source files match the original build receipt.
- The isolated repository builds with the original v100 ABI toolchain and locked local SDK snapshot.
- Header trace stays within this repository and the external Microsoft toolchain/SDK.
- Rebuilt binary is not byte-identical. PE sections, symbol counts, timestamps, path strings and compiler namespace identities are compared in `provenance/rebuild-comparison.json`.
- No changes were made to the original source, installed mod/DLL or saves. No native Kenshi tests and no publication were performed.

## Documented limitations — not a certification of complete GPL compliance

1. **Release identity resolved:** owner explicitly confirmed build 109 is the player-distributed V9 on 8 October 2026. No independent Workshop download is claimed.
2. **Owner declaration (8 October 2026):** all original mod code was developed with Codex; interface images were created for the project; armor models originate from official Kenshi. No armor models or interface artwork are included in this source candidate. This declaration does not relicense Kenshi assets or resolve third-party rights. Dependency licenses retain their own scope; no linking exception or “or later” term has been verified merely from a full GPL text.
3. **Generated preferred inputs:** complete the checks in `GENERATED_INPUTS.md`. Game-derived reference tables are preserved because the DLL requires them; their classification and underlying generator data need review. They must not be silently deleted to make an incomplete package look clean.
4. **Runtime resources:** the inventory lists every excluded mod resource/data file. Editable artwork/model sources, font notices, furniture permission scope, and any Kenshi-derived resources are not resolved by shipping the DLL source. A matching complete mod package is required for installation and gameplay. Do not apply the root GPL indiscriminately to these items.
5. **Runtime library corresponding source:** determine whether and how the exact RE_Kenshi/KenshiLib and their transitive sources must accompany the chosen distribution. Immutable references are useful evidence but not, by themselves, proof of satisfying a required source offer. RE_Kenshi's top-level source zip omits recursive submodule contents. Include exact dependencies/notices or otherwise establish a valid delivery arrangement before claiming completeness.
6. **SDK provenance:** this is a mixed, locally adapted SDK; retain its dependency hashes and modification notice. Public availability of reconstruction headers is not a legal opinion on rights in proprietary game implementation. No game executable or proprietary implementation binary is being republished here.
7. **Reproducibility:** the preparation proves recorded input identity and a successful rebuild, not byte-for-byte or in-game equivalence. Resolve any remaining unexplained binary differences before representing stronger guarantees.
8. **Security:** automated pattern scans do not guarantee absence of all secrets or confidential content. Review the retained third-party public contact/copyright information and newly added material. Keep `_build/` and the entire sibling `verification-private/` out of Git.

The GPL may require the appropriate corresponding source, notices, license permissions, and source-access arrangements for recipients of a covered binary; it does not specifically require GitHub. A link to RE_Kenshi alone would not provide The Mercenarie's source or its local SDK adaptations. See the [GPL text](https://www.gnu.org/licenses/gpl-3.0.html), especially sections 1 and 4–6, and the [GNU FAQ](https://www.gnu.org/licenses/gpl-faq.html#CompleteCorrespondingSource). A qualified lawyer should address disputed combined-work, game-library/system-library and resource questions.

## Current authorization

The owner requested preparation of a local commit and annotated tag `v9-dev.109`, with the limitations above retained. No push is authorized. These limitations do not prevent creating a reviewable local commit; they remain relevant to any claim of complete GPL compliance. Generated factual reference tables, reconstructed API headers and required import libraries remain included and explicitly identified, not described as original independent artwork.
