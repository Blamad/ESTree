# T13 — Modernization gate

**Depends on:** T00–T12 (T11/T12 may be explicitly waived by the owner) · **Size:** S · **Branch:** `task/T13-modernization-gate`

## Goal

Close the program: remove Boost from the build, verify every global constraint and the
acceptance criteria of all earlier tasks on one clean checkout, and leave the docs describing the
repository as it now is.

## Context to read

- `docs/PLAN.md` (Global Constraints, Definition of done), all `REPORT.md` files in `docs/tasks/*/`
- `docs/DECISIONS.md`, `docs/BACKLOG.md`

## Scope

In:
1. Remove Boost from `CMakeLists.txt` (include dirs, `BOOST_LIBRARIES`, `link_directories($ENV{BOOST_LIB}...)`); `grep -rni boost . --include=*.cpp --include=*.h --include=CMakeLists.txt --include=*.json --include=*.md` (excluding `Vendor/`, `build/`) must be explained line by line (history mentions in docs are fine).
2. Fresh-clone verification: `git clone --recursive` into a temp dir, then the documented commands from `README.md` (configure, build Debug+Release, `ctest`, `treegen_cli` run). Paste outputs.
3. Constraint audit: go through every line of *Global Constraints* and *Review Focus* in `docs/PLAN.md` and cite the test/command proving it; list unmet ones.
4. Docs refresh: `README.md` (new build instructions, requirements, test/CLI usage, link to `docs/`), `CLAUDE.md` (remove "Known defects" entries that are fixed; update layout: `ESEngine/TreeGen/` now exists; build section matches reality), `docs/architecture/overview.md` (final state), `docs/PLAN.md` task statuses (this task is the one allowed to update the table, based on the reports).
5. Produce `docs/tasks/T13-modernization-gate/REPORT.md` with a one-page summary: what was achieved, remaining debt, recommended next steps (draw from `BACKLOG.md`), and the human-verification checklist collected from all reports.

Out of scope: new features.

## Owned files

`CMakeLists.txt` (Boost removal), `README.md`, `CLAUDE.md`, `docs/**` status sections.

## Acceptance criteria

1. No Boost in the build; project configures without `BOOST_LIB` set.
2. Fresh clone builds and tests green (evidence in report).
3. Every Global Constraint has a citation or is listed as unmet with an owner.
4. `docs/PLAN.md` reflects reality; the consolidated human-verification checklist exists.

## Verification

Automated + the consolidated human checklist for the owner.
