# Generated inputs — follow-up investigation

Run `python tools/check_generators.py`. It generates only in a temporary directory, compares all three outputs byte-for-byte, and leaves the source snapshot unchanged. All three checks passed on 8 October 2026.

- **CleanupOwnedDefinitions.generated.h:** recovered `fcs-inventory.json` from the historical cleanup work and the extraction at `add-coverage.py:6-8`. The 158 added record IDs exactly reproduce the header, with its original line endings. The public input is the extracted ID list, not the whole game/mod audit.
- **RealEstateBaseline.generated.h:** recovered the original FCS audit; the original generator reproduced the exact header. A minimal projection retains only fields actually consumed, omitting unrelated game records/fields. The generator-only excerpt has no translation or installation side effects. Game-derived table rights remain a scope question; no game database is included.
- **LocalizationDefaults.generated.h:** searched 629 English catalogues; recovered catalogues have the same 2,794 English keys/values. The historical generator does not reproduce the header, which was also directly patched (e.g. contract-board `prepare.py:56-57`). A NEW, explicitly labelled lossless editable JSON extraction preserves 11,957 ordered rows, duplicates and encoding choices and regenerates the original header exactly. It is not represented as a historical source. Recovered catalogues and the historical generator are retained too. Whether these forms fully satisfy the preferred-form requirement, rather than the complete historical patch pipeline, remains a review question.
- **SaveBuild.generated.h:** a directly editable text declaration of the fixed build identifier; no missing complex generation input is needed to edit it.

`generators/inputs/PROVENANCE.json` identifies the recovered historical inputs and their hashes. The new checker never executes the broader historical patch scripts. Those scripts modified the development tree and are not needed to build the stored 109 snapshot.

GPLv3 section 1 concerns the preferred form for modification and necessary source/scripts, not preservation of every historical intermediate. Successful regeneration is technical evidence, not a legal determination of preferred form or ownership. https://www.gnu.org/licenses/gpl.en.html
