---
name: audit-google-typescript-style
description: Audit TypeScript and TSX code against the Google TypeScript Style Guide using GTS plus supplemental analysis, then propose a short grouped remediation plan for explicit human approval before modifying approved source files. Invoke only when the user explicitly requests this audit or invokes $audit-google-typescript-style. Do not use for ordinary code reviews, pull-request reviews, linting, or implementation tasks.
---

# Audit Google TypeScript Style

Perform a specialist Google TypeScript Style audit with GTS as the deterministic
baseline. This skill has two explicit phases: plan, then apply after approval.

## Invocation boundary

Use this skill only for an explicit request such as:

```text
$audit-google-typescript-style
Audit the TypeScript codebase and propose approved style fixes.
```

Do not invoke it implicitly as part of `review-change`, `clean-code-review`,
`pr-review`, a normal code review, or a generic linting request.

Invoking the skill is not approval to modify code. Keep the audit and plan
phase read-only. Require a separate affirmative human response to the exact
plan before entering the apply phase.

## Phase A: audit and approval plan

1. Read repository instructions and identify each TypeScript package, package
   manager, `package.json`, `tsconfig` file, lint configuration, generated-code
   directories, local exceptions, and the current working-tree state.
2. Define the in-scope `.ts` and `.tsx` files. Include tests unless explicitly
   excluded. Exclude dependencies, build output, vendored code, and generated
   code only with repository evidence.
3. Run the existing package scripts before making judgments. For this
   repository's `clip-extension`, run `npm run check-types`, `npm run lint`,
   and `npm run format:check`.
4. Run `scripts/check_google_rules.mjs` for each TypeScript package.
5. Perform the agent judgment pass over documentation, naming, imports, type
   design, exports, static state, comments, and local consistency.
6. Produce a short remediation plan grouped by source-file collection and rule
   family. Do not list hundreds of repetitive findings individually. Include:
   - approved scope and excluded files;
   - grouped files or directories;
   - exact style changes proposed;
   - whether a change is formatter-only or manual;
   - non-goals and behavior-preservation constraints; and
   - post-approval validation commands.
7. Stop and ask the human to approve, reject, or revise this exact plan. Do not
   edit files, run write-mode formatters, or invoke automated fixes before
   approval.

If the plan depends on missing scope, a project-policy decision, uncertain
generated-code status, or a behavior/public-API/architecture change, ask for
clarification instead of silently expanding the plan.

## Strict documentation contract

Treat documentation as a style requirement for every documentable symbol,
whether it is public or private. This includes module-level functions, classes,
interfaces, type aliases, enums, constructors, methods, accessors, properties,
and callable or member signatures. Do not exempt a private or internal symbol
merely because it is not exported. Truly generated code is excluded only when
the repository provides evidence that it is generated and out of scope.

Each JSDoc block must contain:

- a concise summary sentence on the first line, terminated as a complete
  sentence;
- an `@param` entry describing every meaningful parameter, including optional,
  rest, and private parameters; and
- an `@return`/`@returns` entry describing every non-`void` return value.

Document thrown errors with `@throws` when the function's error behavior is
part of its contract. Do not accept a placeholder, a restatement of a name, or
an empty JSDoc block as documentation. For declarations with no parameters or
with a `void` return, the corresponding tag is not required, but the summary
must still describe the behavior. Missing blocks or missing applicable
parameter/return descriptions belong in the proposed remediation plan and are
style fixes only unless the wording requires a human API or behavior decision.

## Phase B: approved remediation

Enter this phase only after explicit approval of the plan. Treat approval as
limited to the named files, rule families, and non-goals.

1. Re-check the working tree and re-audit the approved scope. If relevant files
   changed since the plan, stop and present an updated plan.
2. Apply only the approved style changes. Preserve runtime behavior, public
   APIs, tests, and unrelated user changes. Do not alter dependencies,
   configuration, or generated sources unless explicitly approved.
3. Use the repository's existing GTS formatter/fixer for approved files where
   its scope is safe. Do not run a package-wide write-mode formatter for a
   partial approval unless the human approved that broader scope.
4. Make remaining AST, documentation, import, and design corrections manually.
5. Re-run GTS/type checks and the supplemental checker. Run compile or tests
   when the edits could affect behavior; style-only edits still require the
   repository's applicable compile/type validation.
6. Inspect the exact diff for scope expansion, accidental behavior changes,
   weakened tests, and formatter churn.
7. Report applied changes, validation evidence, residual findings, and any
   items that require a new approval. Do not merge, commit, or release.

## Deterministic and supplemental checks

Treat passing GTS and TypeScript checks as evidence, not proof. The supplemental
checker must identify default exports, `export let`, namespaces, import-equals
require, `var`, `const enum`, banned constructors, `debugger`, `with`, `eval`,
`Function`, TypeScript suppression comments, `for...in` filtering reviews, and
documentable declarations lacking JSDoc, including private declarations.

Use the TypeScript compiler API and report relative paths, one-based line and
column, rule, severity, and evidence. Judgment items remain review findings
until the agent decides whether the guide and repository context establish a
violation.

## Style judgment

Apply the guide's normative words precisely: `must` and `must not` are
violations; `should` and `should not` require context-sensitive judgment. Do
not turn examples or personal preferences into requirements. Review naming,
import choices, source ordering, JSDoc completeness and quality, type
inference, `any`, interfaces, classes, complex types, static state, comments,
deprecations, and generated-code exceptions. The documentation contract above
is an explicit project audit policy and applies to private symbols as well as
exports.

## Plan and completion reports

The plan report must end with `APPROVAL REQUIRED` unless no changes are needed.
The apply report must include exact commands and results, approved scope,
changed files, residual findings, and one verdict: `COMPLETED`, `PARTIALLY
COMPLETED`, or `CLARIFICATION REQUIRED`.

Never claim compliance solely because GTS passes. Human approval authorizes only
the approved remediation plan and never authorizes merge or release.
