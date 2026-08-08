# GitHub Issue: [Issue Name]

**Status:** Draft | In Review | Approved  
**Author:** [Name or Team]  
**Last Updated:** [YYYY-MM-DD]  
**Target Developer Level:** Junior / Mid / Senior

---

## 1. Overview

Provide a clear, self-contained description of the feature in 3–5 sentences. Cover:
- What the feature does.
- The specific problem it solves.
- Who benefits from it and how.

> **Example:** "The CSV Export feature allows authenticated users to download their transaction history as a `.csv` file directly from the Dashboard. Currently, users must contact support to obtain this data, creating a bottleneck. This feature eliminates that dependency and reduces support ticket volume."

---

## 2. Goals and Non-Goals

### Goals
List only what **must** be achieved in this implementation. Keep the list minimal and concrete.

- [ ] Goal 1 (e.g., "Users can trigger a CSV export from the Dashboard.")
- [ ] Goal 2 (e.g., "Exported file includes all transactions from the past 12 months.")
- [ ] Goal 3

### Non-Goals
Explicitly state what is **out of scope** for this task. This is critical for preventing scope creep.

- Non-Goal 1 (e.g., "Custom date-range selection is not included in this version.")
- Non-Goal 2 (e.g., "PDF export format is not supported.")
- Non-Goal 3

---

## 3. Technical Approach

Describe **how** the feature will be implemented at a high level. This section should answer:
- What architectural pattern or design is being used?
- Which existing modules, services, or utilities are being leveraged?
- Why is this a simple and the most appropriate approach?

**Key decisions and rationale:**

| Decision | Rationale |
|---|---|
| [e.g., Use existing `DataExporter` service] | [e.g., Avoids duplicating serialization logic already tested in production.] |
| [e.g., Generate the file server-side] | [e.g., Keeps sensitive data off the client and simplifies browser compatibility.] |

**Known constraints or conventions to follow:**
- [e.g., All new API endpoints must be versioned under `/api/v2/`.]
- [e.g., Follow the repository's error-handling pattern using `AppError`.]

---

## 6. Conventions

List every convention, internal module, or codebase pattern the developer must be aware of for this issue.

### Internal Modules and Patterns
- **Must use** `services/transaction_repo.py` for all database access. Do not write raw queries.
- **Must follow** the error handling pattern defined in `core/exceptions.py`. Raise `AppError` with the appropriate error code.
- [Add any other mandatory conventions.]

### Skill or Domain Guidelines
- [e.g., "This feature uses the Vulkan rendering API. Ensure the Vulkan skill guidelines are followed for resource management and synchronization."]
- [e.g., "All UI components must use the design system tokens defined in `styles/tokens.css`."]
