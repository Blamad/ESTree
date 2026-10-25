# Working with tasks

Each task has a directory `docs/tasks/Txx-slug/`:

| File | Written by | Purpose |
|---|---|---|
| `BRIEF.md` | planning session | Goal, context, scope, owned files, acceptance criteria. **Read-only for you.** |
| `PLAN.md` | **you**, before coding | Step-by-step implementation plan (checkboxes), decisions, open questions. Starts as a stub. |
| `REPORT.md` | **you**, at the end | What changed, evidence, deviations, follow-ups, human-verification items. Create from `_templates/REPORT.template.md`. |
| `notes/` (optional) | you | Scratch notes, captured outputs, benchmark logs. Keep them small. |

## Lifecycle

1. **Claim.** Read `/CLAUDE.md`, `docs/PLAN.md` (global constraints, review focus, ownership
   matrix), `docs/architecture/overview.md`, `docs/architecture/treegen-design.md` (if the task
   touches `treegen`), and the `BRIEF.md`. Check that every task in "Depends on" is `done` in
   `docs/PLAN.md` (or merged); if not, stop and report.
2. **Branch.** `git worktree add ../ESTree-Txx -b task/Txx-slug modernization` (`modernization` is the integration branch; never base on or merge into `master`) (or a plain
   branch if worktrees are unavailable). Submodules: `git submodule update --init` in the worktree.
3. **Plan.** Fill `PLAN.md` (keep the header block; set `Status: in-progress`). Use the
   `superpowers:writing-plans` conventions: exact files, exact signatures, test names with
   assertions, commands with expected output, small steps, a commit per step group. Commit it
   (`Txx: add plan`) before implementing. If the plan contradicts the BRIEF, say why in a
   "Deviations" section; if it contradicts `treegen-design.md`, update that file too.
4. **Implement.** TDD (`superpowers:test-driven-development`): failing test → minimal code → pass.
   Stay within the files the BRIEF says you own. Keep the engine buildable at every commit.
5. **Verify.** `superpowers:verification-before-completion`: run the build and the tests fresh,
   paste the command and the relevant output lines into `REPORT.md`.
6. **Report.** Write `REPORT.md`, set `Status: review` in `PLAN.md`. Don't touch `docs/PLAN.md`
   status — the orchestrator does after reviewing.

## Rules of engagement

- One task per branch, one branch per agent. Don't fix neighbouring tasks' problems; list them under *Follow-ups*.
- Unknown or ambiguous? Make the smallest reasonable choice, write it down in `PLAN.md` →
  *Decisions*, and if it affects others add a `docs/DECISIONS.md` entry.
- Interface changes to `treegen` are allowed only by editing `docs/architecture/treegen-design.md`
  in the same commit; announce it in `REPORT.md` → *Interface changes*.
- No visual verification by agents. Describe what a human should look at instead (see template).
- Don't commit generated files (`build/`, `.obj` outputs, logs) unless the BRIEF says they are golden data under `tests/golden/`.
- Time-box: if a step takes more than ~3 attempts, stop, write down what you tried in `PLAN.md` → *Blockers*, and report instead of widening scope.

## Templates

- [`_templates/PLAN.template.md`](_templates/PLAN.template.md)
- [`_templates/REPORT.template.md`](_templates/REPORT.template.md)
