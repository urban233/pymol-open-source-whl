# Technical Writing Style Reference

Distilled from Google's Technical Writing courses and the Google developer
documentation style guide. Apply this to the prose in a reviewer-facing
technical document -- a specification, brief, design, delivery plan,
launch plan, or similar artifact -- not to code, commit messages, or
agent-facing instructions. These are guidelines: depart from one where it
makes a specific passage clearer, but apply the departure consistently
within the document, and never as an excuse to drop a required condition,
warning, or exception.

The reader is a named human reviewer, stakeholder, or the accountable
owner deciding whether to accept the document -- not another agent
operating under a token budget. Write for that reader.

## Contents

1. Reader-first foundations
2. Opening and structure
3. Headings and navigation
4. Progressive disclosure and paragraphs
5. Lists and tables
6. Sentence-level language
7. Terminology and jargon
8. Global and inclusive writing
9. Formatting conventions
10. Diagrams
11. Self-check before saving

## 1. Reader-first foundations

- Name who the document is for and their relevant prior knowledge, not just
  their job title -- a licensing reviewer and an ML-evaluation reviewer need
  different things from the same document.
- Counter the curse of knowledge: an unexplained acronym, an implicit
  implementation detail, or an assumed conceptual leap that feels obvious to
  the author is often not obvious to the reviewer.
- Organize around what the reader needs to decide or do, not around the
  order in which the author figured it out.
- Write in a direct, approachable, professional tone. Skip marketing
  adjectives ("blazing fast," "effortless," "best-in-class") and skip
  pre-announcing what the document will cover ("In this document, you will
  learn...") -- just say it.

## 2. Opening and structure

- Open with the important result: the recommendation, decision, or outcome
  and why, not history or throat-clearing. Assume some readers read only
  the Summary.
- State scope explicitly. State non-scope only when a reasonable reader
  might otherwise expect the document to cover it.
- Order sections by the reader's dependency chain: context and constraints
  before the proposal, the proposal before alternatives, alternatives
  before risk and rollout.
- If the document has grown to cover more than one independently
  reviewable scope, check whether the owning skill defines a split rule
  before compensating with denser prose to fit everything into one file
  (`design-solution`'s parent/child design rule is the current example).

## 3. Headings and navigation

- Use sentence case: `## Configure the authentication plugin`, not
  `## Configure The Authentication Plugin`.
- Make headings specific and scannable on their own, out of context.
- Never place a heading directly above another heading. Put at least one or
  two sentences of real content under every heading before its first
  subheading, list, or table.
- Do not number headings to imply sequence; use ordered lists for that.

## 4. Progressive disclosure and paragraphs

- Introduce a term or concept close to where it is first needed, not all
  up front.
- Keep each paragraph to one topic and roughly three to five sentences.
  Split a paragraph that is growing a second topic or exceeding about seven
  sentences.
- Open each paragraph with the sentence that states its point; readers skim
  first sentences.
- Break up dense prose with a list, table, or diagram once a sentence is
  carrying more than two or three coordinated ideas.

## 5. Lists and tables

- Numbered list: only when order, sequence, or priority changes the
  meaning. Start each item with an imperative verb for a procedure.
- Bulleted list: a nonsequential set of items, options, or facts.
- Table: readers must compare multiple properties across several
  structured items (component ownership, API contracts, alternatives, and
  open-question tables are the right use).
- Drop a column where nearly every row repeats the same value (an owner
  column that says the same name eleven times). State that fact once in
  the sentence introducing the table instead. A column earns its place by
  varying row to row.
- Check every table in the document against this rule, not only the one
  that originally prompted it. A table with the same shape elsewhere in
  the same document -- an open-questions table with its own all-identical
  owner column, say -- usually has the same defect and is easy to miss
  once the first instance is already fixed.
- Prefer roughly four or five columns. When one comparison genuinely needs
  more than that, it is usually because some of those columns hold a
  sentence or two of narrative -- guarantees, error behavior, test notes --
  rather than a short comparable value. Move that narrative into a short
  bulleted block under each row, and keep in the table only the columns
  worth scanning side by side (name, owner, a short status). Do not
  compensate by cramming a paragraph into a cell instead.
- If a wide table's rows belong to several different subsystems, that is
  often a document that needs the parent/child split (see "Opening and
  structure" above), not a table that needs reformatting -- splitting the
  document usually shrinks the table on its own, since each child then
  covers only its own contracts.
- A decision or alternatives table stays complete: keep every option
  actually considered and rejected, even if that means more rows. Row
  count is not what makes a table hard to read; forcing the reader across
  many dense columns per row is. Tighten a cell's wording before cutting a
  row for length.
- Do not write a list as a single sentence joined by commas and "and" when
  each clause is really its own item -- that is a list wearing prose. A
  sentence chaining more than two sequential or enumerable actions should
  become a list.
- Keep list items grammatically parallel (all imperative verbs, all noun
  phrases, or all full sentences -- not a mix).
- Introduce a list or table with a complete sentence, not a fragment the
  list has to complete.
- Place a condition before the instruction it governs: "If the build fails,
  check the log." not "Check the log if the build fails."

## 6. Sentence-level language

- Second person for the reader ("you"), never "the user" for the reader
  themself. Reserve "user" for the end user of the software being
  described.
- Active voice: name the actor. "The server returns a 404." not "A 404 is
  returned." Passive voice is acceptable only when the actor is genuinely
  unknown or irrelevant, and the surrounding sentence still makes the
  action clear.
- Present tense for current behavior and standing decisions. Use another
  tense only for a real temporal distinction (a past constraint, a future
  gate that has not opened yet) -- not as a habit.
- One idea per sentence. Split a sentence with more than one independent
  clause, or convert it to a list.
- Replace "there is" / "there are" openers with the real subject and verb:
  "A configuration file specifies the port." not "There is a configuration
  file that specifies the port."
- Prefer concrete, specific verbs over generic ones (`be`, `occur`,
  `happen`, `perform`, `utilize`, `leverage`) and over nominalizations
  ("the compiler generates an error," not "an error happens" or "there is
  error generation").
- Avoid subjective or promotional modifiers with no supporting evidence:
  `easy`, `simple`, `simply`, `obviously`, `clearly`, `fast`, `best`. State
  the measurement or condition instead.
- Avoid directional references (`above`, `below`, `left-hand side`) and
  color-only references; they break under reflow and for screen readers.
  Name the section or artifact instead.

## 7. Terminology and jargon

- Pick one term per concept and use it consistently. Do not silently vary
  between synonyms ("directory" / "folder," "argument" / "parameter").
- Define or expand an acronym, metric, or technique on first use in the
  document if any named reviewer type might not already know it -- a
  licensing or domain reviewer will not know ML shorthand (IoU, RMSD, DPO,
  pass@k), and an ML reviewer will not know domain shorthand. Example:
  "IoU (intersection over union)."
- A term's first use resets per document when several documents were split
  from a former single one. A definition that lives only in a sibling
  file does not satisfy this rule for the file being read -- each
  document must stand on its own for its own named reviewer.
- Avoid idioms, buzzwords, and culturally local references ("ballpark
  figure," "under the hood," "low-hanging fruit"); they do not translate
  and slow down non-native readers.
- Prefer a single clear verb over a phrasal verb when one exists
  (`configure` rather than `set up` a value, `remove` rather than `leave
  out`) -- but keep a phrasal verb that is itself the standard term for the
  action (`set up an account`, `sign in`, `log in`), since replacing those
  makes the document less recognizable, not more.

## 8. Global and inclusive writing

- Use short, common words (`start` not `commence`, `use` not `utilize`,
  `so` not `consequently`) and standard subject-verb-object order.
- Use US English spelling and the Oxford (serial) comma in lists of three
  or more items.
- Write dates and units unambiguously: `2026-08-20` or `August 20, 2026`,
  never `08/20/26`. Include a time zone where the time matters.
- Use singular "they" or second person instead of gendered pronouns or
  "he/she." Use "person-hours," not "man-hours."
- Prefer inclusive technical terms already in use in the codebase and
  ecosystem (for example `primary`/`secondary`, `allowlist`/`denylist`)
  over exclusionary alternatives.

## 9. Formatting conventions

- Code font for identifiers, filenames, commands, flags, values a reader
  types, and other literal machine-readable strings.
- Bold for UI labels, matched to the actual label text: "Click **Save**."
- Descriptive link text naming the destination -- never "click here" or a
  bare URL as prose.
- Do not put a link inside a heading.

## 10. Diagrams

- Write the takeaway sentence first, then make the diagram earn it; keep a
  short lead-in sentence before the diagram in the document, and a caption
  or explanation after it.
- Cap a single diagram at roughly one paragraph's worth of information --
  one architectural idea or one end-to-end flow. Split a system with
  several major stages into an overview diagram plus separate subsystem
  diagrams rather than one diagram carrying everything.
- The document must still make sense if the diagram is skipped; do not put
  a fact only in the diagram.

## 11. Self-check before saving

- [ ] The opening states the recommendation, decision, or outcome and why
      in the first few sentences.
- [ ] No sentence chains more than two sequential or enumerable actions
      that could be a list instead.
- [ ] Every acronym or specialist term a named reviewer type might not know
      is defined or expanded on first use.
- [ ] Every heading has real content directly beneath it before the next
      heading.
- [ ] Tables are used for comparison, not as a dumping ground for prose
      that does not fit elsewhere.
- [ ] No table column repeats nearly the same value in almost every row,
      and no table forces the reader across more dense columns than one
      comparison actually needs.
- [ ] The document covers one coherent scope its named reviewers can
      evaluate without reading substantial unrelated detail -- and if not,
      the owning skill's own splitting rule (where one exists) has been
      applied.
