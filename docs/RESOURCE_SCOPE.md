# Runtime resource scope

Every installed file is individually classified in `provenance/runtime-resource-inventory.json`. None of those artwork/data files is read by the isolated DLL compilation: the successful compiler trace is confined to the retained source, SDK, Boost and Microsoft toolchain. Runtime necessity is a separate question.

1. DLL compilation inputs: all 395 source files, generated tables, compatibility header, SDK, Boost and tool scripts are retained. Generated tables compiled into the DLL belong in the source review; do not delete them to avoid rights questions.
2. Editable runtime data: localization JSON is included. UI XML, routes and FCS mod data are not automatically independent merely because they have another extension. Review whether each is part of the covered work or a separate work; if covered, provide the actual editable form and needed generation tools. XML may already be editable source; compiled FCS/model formats may require additional originals.
3. Independent artwork: images, meshes, skeletons and fonts are not automatically GPL because a GPL program displays them. Their own provenance/license and any combined-work question govern their treatment. If an asset is covered and a layered image/Blender/project file is the preferred editable form, a flattened PNG or compiled mesh alone may be insufficient. No unverified asset has been copied into the candidate.
4. Libre Caslon Text: installed OFL notice identifies the font authors and SIL OFL 1.1. This is a separate license, not GPL; font binaries and their notice are not needed for DLL compilation. Existing mod packaging should preserve the OFL notice. No claim that every font upstream editable file has been checked.
5. Mechanica chair: the installed notice records permission to integrate chair4 (local backed-chair mesh/XML/bin), not permission to relicense it or distribute original editable project files. Obtain the actual permission wording if expanding distribution beyond that integration. Do not publish these assets here by inference.
6. Game-derived map/mesh/texture/FCS information: rights and provenance not established by an extension or a local copy. Keep unverified proprietary resources out of this source repository. Do not assume this alone resolves compliance if the resources form part of a covered combined work.

GNU GPLv3 section 5 distinguishes an aggregate from a combined work. These are review categories, not final legal classifications: https://www.gnu.org/licenses/gpl.en.html ; https://www.gnu.org/licenses/gpl-faq.en.html

## Owner clarification, 8 October 2026

Original code was developed with Codex, interface images were created for the project, and armor models originate from official Kenshi. No Kenshi armor meshes, textures, skeletons, game databases or executables are included here. The only PNG is upstream Boost documentation. Required SDK reconstruction headers/import libraries and the documented game-derived factual tables remain; their provenance and legal scope are not concealed by the asset exclusion.
