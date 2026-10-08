# Proposed future source-release pipeline (not implemented)

1. Assign an immutable release identity to the intended player build. Freeze source, generated inputs, dependency revisions and licenses before compiling.
2. Build in an isolated directory. Record compiler and linker versions/options, hashes of every source/input, the complete include trace, runtime dependency versions, and the resulting DLL/package SHA-256.
3. Assemble a source staging tree from explicit input manifests, not a generic copy of the development directory. Include local third-party patches, required generated/preferred inputs, build/install scripts and notices. Do not silently omit a required file that fails a rights or secret check.
4. Scan text and binary metadata for secrets and private paths, and check every shipped dependency/resource against a rights inventory. Stop on unresolved issues; produce redacted findings.
5. Rebuild from the staging tree with no access to the working tree. Compare exports/imports, sections and symbols; account explicitly for timestamp/path nondeterminism. Native tests remain a separate gate.
6. Generate a complete source archive and manifest. Associate it with the exact binary archive and publish source-access instructions to binary recipients through a legally appropriate mechanism.
7. After explicit human approval, create a source commit/tag for the **actual distributed version**. For example: `v9-dev.109` only if that development build is intentionally released, `v9.1` only for the real V9.1 release. Never reuse or move a public release tag.
8. Keep the release receipt and source availability stable for the required duration. A future dependency or license change should reopen review rather than inherit a previous approval.

No CI workflow, scheduler, publication automation, Git tag, push or public release was created in this preparation.
