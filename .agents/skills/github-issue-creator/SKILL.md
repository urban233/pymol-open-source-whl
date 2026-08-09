---
name: github-issue-creator
description: >
  Creates structured, actionable GitHub issues following a standardized template, ready for developer handoff.
  Activate this skill whenever a user wants to turn a feature idea, bug, or task into a GitHub issue — even if they phrase it as "create an issue for X", "write up this feature as a ticket", "draft a GitHub issue", "make this a ticket", or "I need to file an issue". Also activate when a feature has already been discussed in the conversation and the user says something like "turn this into an issue" or "make this a GitHub issue". Do not wait for the user to say the exact words "GitHub issue" — if the intent is to produce a structured ticket or issue for a feature or task, this skill applies.
---

# GitHub Issue Creator

This skill produces a structured GitHub issue following a fixed template. The output is detailed enough for a developer to implement the feature without needing follow-up clarification.

---

## Phase 1: Context Detection

**Before asking anything**, scan the conversation history:

- If a feature, bug, or task has already been described in detail → use it as the starting point and move directly to Phase 2 with that context pre-loaded.
- If no feature has been described → ask the user: *"What feature or task should I turn into a GitHub issue?"* Wait for their answer, then move to Phase 2.

---

## Phase 2: Clarification (3–5 Questions)

Once you have a starting feature description, ask **3 to 5 targeted questions in a single message** to fill in any gaps before drafting. Do not draft the issue until you have answers.

Choose questions that are most relevant to the specific feature, drawn from these categories:

**Always ask at minimum:**
1. **Scope / Non-Goals** — What is explicitly out of scope for this issue? What should a developer *not* try to solve here?
2. **Technical approach** — Is there a preferred implementation approach, existing service/module to reuse, or architectural constraint to follow?

**Ask as needed based on what's unclear:**
3. **Target developer level** — Is this intended for a junior, mid-level, or senior developer?
4. **Conventions** — Are there specific internal modules, patterns, or file locations the developer must use (e.g., a specific repo service, error-handling pattern, naming convention)?
5. **Goals confirmation** — Can you confirm the 2–3 must-have outcomes for this issue? What does "done" look like?

Ask all questions in one organized message. Number them clearly. Do not ask redundant questions if the answer is already evident from the conversation.

---

## Phase 3: Draft the Issue

Once you have answers, generate the GitHub issue using **exactly** the template in `references/issue-template.md`.

**Quality bar:** The issue must be self-contained. A developer assigned to it should be able to implement the feature without asking any follow-up questions.

**Completeness checklist before delivering:**
- [ ] Overview is 3–5 sentences and covers what, why, and who.
- [ ] Goals are concrete and checkboxed; Non-Goals are explicit.
- [ ] Technical Approach includes a decision table with rationale.
- [ ] Conventions section names specific files/modules/patterns where relevant.
- [ ] Status, Author, Last Updated, and Target Developer Level are filled (use placeholders if unknown).

Present the issue as a markdown code block so the user can copy-paste it directly into GitHub.

---

## Reference Files

| File | Purpose | When to Read |
|---|---|---|
| `references/issue-template.md` | Canonical GitHub issue template | Before drafting (Phase 3) |