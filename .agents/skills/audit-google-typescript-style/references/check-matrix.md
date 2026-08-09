# Google TypeScript Audit Check Matrix

## Authority

Use the [Google TypeScript Style Guide](https://google.github.io/styleguide/tsguide.html) as the style authority. Treat its normative terms as requirements; examples are illustrative and must not become additional rules.

## Check ownership

| Area | Primary check | Result interpretation |
| --- | --- | --- |
| Type correctness | Repository TypeScript script | A failing compiler check is a finding. |
| Formatting and common lint rules | Repository GTS scripts | Report tool output; do not duplicate it. |
| Syntax bans | `scripts/check_google_rules.mjs` | Report confirmed AST or lexical violations. |
| Documentation and type design | Agent judgment plus supplemental checker | Inspect every public and private documentable symbol, including summary, parameters, returns, and throws. |
| Generated code | Repository evidence plus agent judgment | Exclude only with evidence. |

## Supplemental hard rules

The checker confirms default exports, `export let`, namespaces, import-equals-require, `var`, `const enum`, banned constructors, `debugger`, `with`, `eval`, `Function`, and prohibited TypeScript suppression comments.

## Supplemental review rules

The checker reports `for...in` and missing or incomplete JSDoc for every
documentable declaration, including private members, for agent review. A
generated declaration is excluded only with repository evidence. The agent
must also verify that each JSDoc block has a complete first-line summary,
`@param` entries for meaningful parameters, and `@return`/`@returns` entries
for non-`void` results. These findings still require context for generated or
tooling code and other documented project exceptions.

## Scope policy

Do not inspect `node_modules`, build output, coverage output, vendored sources, or generated sources unless explicitly included. Include source tests by default and record every exclusion.
