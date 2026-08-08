# GEMINI.md

Follow `AGENTS.md` as the authoritative repository guidance. In addition,
maintain `docs/architecture/` as CLIP's durable architectural knowledge base.

## Architecture knowledge base

Create or update the relevant Markdown knowledge-base record whenever work
creates, validates, changes, retires, or discovers an architectural decision,
external integration boundary, compatibility result, or legacy-system risk.
These records are required deliverables when applicable.

- Record accepted, consequential decisions as ADRs in
  `docs/architecture/decisions/`, following that directory's ADR convention.
  Supersede an ADR when a decision changes rather than silently rewriting
  history.
- Record host and transport validation in a focused note such as
  `docs/architecture/chatgpt-mcp-probe.md`. Include the tested setup, results,
  outstanding evidence, data-exposure boundary, and the decision the evidence
  enables or blocks.
- Record a legacy component's risk and disposition in
  `docs/architecture/legacy-component-inventory.md` before it is reused,
  quarantined, replaced, or removed. Include reuse conditions where relevant.
- Keep records factual, scoped, dated when recording validation, and linked to
  related code, tests, roadmap tasks, and ADRs. Never include credentials,
  secrets, or private scientific data.
- Do not create boilerplate documentation for routine edits with no
  architectural, integration, validation, or legacy-disposition impact.
