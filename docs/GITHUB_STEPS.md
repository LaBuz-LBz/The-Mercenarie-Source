# GitHub publication steps — do not execute until approved

The destination is https://github.com/LaBuz-LBz/The-Mercenarie-Source. It already contains a README, GPLv3 LICENSE and .gitignore. The prepared LICENSE was compared with the current remote text and retained unchanged. Preserve that repository's history; do not force-push or replace its root history.

First resolve `PUBLICATION_REVIEW.md` and get the owner's explicit publication approval. Then:

1. Clone the existing repository into a new, separate checkout. Create a preparation branch such as `prepare-v9-dev-109`.
2. Copy the contents of this **repository/** directory into that checkout, excluding `_build/` and any `__pycache__/`. Never copy the outer preparation directory or `verification-private/`.
3. Run the build and secret checks in the new checkout. Review `git status --short`, `git diff --stat`, `git diff`, and `git diff --cached --stat` after staging. Check that no logs, outputs, private files or missing required libraries are present. Do not blindly use `git add` from the outer preparation directory.
4. Commit the reviewed source snapshot with its exact build identity, original DLL hash and known limitations in the commit message.
5. Only under the approved publication scope, push the preparation branch and review the resulting public changes. Do not force-push.
6. Merge under the approved scope. Create an annotated tag only after establishing the actual version released to players. `v9-dev.109` is a proposal, not a tag already created or a substitute for `v9.0`.
7. If separately authorized, upload the reviewed source archive with its manifest and SHA-256 and link it beside the matching binary download. A public GitHub Release is a separate publication action; it was not performed here.

Example Git commands **for the later approved step**, from a separate chosen working directory:

```powershell
git clone https://github.com/LaBuz-LBz/The-Mercenarie-Source.git publish-checkout
cd publish-checkout
git switch -c prepare-v9-dev-109
# Copy only the reviewed repository/ contents, excluding _build/ and caches.
git status --short
git diff --stat
# Stage reviewed paths, inspect the staged diff, then commit.
git add .
git diff --cached --stat
git diff --cached --check
git commit -m "Prepare corresponding source for V9 development build 109"
# Explicit publication approval required before the following command:
git push -u origin prepare-v9-dev-109
```

No commands in this document were executed against the destination Git repository.
