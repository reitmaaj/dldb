# Acceptance: example corpus

## Must exhibit

- Exactly 100 examples: `NNNN-name.dlx` (98) plus `NNNN-name.steps` (2).
- Every example is positive Datalog, initializes, loads, and passes
  `dldb check --full`.
- Query output, normalized by sorting data rows and separating results with a
  lone `%` line, equals the checked-in `.out` byte for byte.
- `0098-rule-replacement` shows that replacing rules retracts previously
  derived tuples while EDB facts survive.
- `0099-persistence` shows that a fresh load reproduces the same results.
- `dldb example run.sh --bless` is deterministic: a second immediate run
  produces no `.out` diff.

## Must reject

- Any example whose DLX fails to parse, whose evaluation fails, or whose
  normalized output differs from `.out`: `just examples` exits nonzero.
- Any example containing negation, arithmetic, aggregation, or comparison
  syntax: DLX/DLQ reject it.
