# BDD scenarios: example corpus

## SCENARIO X1: a graded showcase

GIVEN one hundred examples ordered from one nullary fact to multi-query
real-world models
WHEN `just examples` runs
THEN every example initializes, loads, passes `check --full`, and its
normalized query output matches the checked-in `.out` file.

## SCENARIO X2: features are represented

GIVEN the example set
WHEN scanned by tier
THEN tiers 1-2 show atomic facts and first rules, tier 3 recursion and closure,
tier 4 typed and Unicode values, tier 5 query capabilities, tier 6 idioms,
tier 7 real-world models, and tier 8 capstones including rule replacement and
persistence.

## SCENARIO X3: lifecycle

GIVEN the `.steps` capstones
WHEN run
THEN `replace` retracts derivations supported only by the old rules, and
reloading reproduces the same query results.

## SCENARIO X4: positive Datalog only

GIVEN every example
WHEN loaded
THEN it uses only finite positive Datalog (no negation, arithmetic,
aggregation, or comparison).
