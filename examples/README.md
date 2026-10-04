# dldb examples

One hundred graded examples, from a single nullary fact to small real-world
models. Each example is a complete, runnable program plus queries and a
checked-in expected result; `just examples` runs them all.

## Layout

```
NNNN-name.dlx     facts and positive rules (DLX)
NNNN-name.dlq     one ?- query per line
NNNN-name.out     expected normalized output
NNNN-name.a/.b    program variants (rule-replacement example)
NNNN-name.steps   scripted lifecycle (replace / reload)
gen.def           authoring source for the corpus
generate.sh       expands gen.def into .dlx/.dlq/.a/.b/.steps
run.sh            harness: compare, or --bless to regenerate goldens
```

Run one by hand:

```sh
just build
./build/dldb init /tmp/demo
./build/dldb load /tmp/demo examples/0031-parent-ancestor.dlx
./build/dldb query /tmp/demo '?- ancestor("alice", X).'
./build/dldb check --full /tmp/demo
```

## Expected output

Within each query result the column-name header (or a single `true`/`false`)
is kept and the data rows are sorted, because dldb query row order carries no
guarantee. Results of the queries in one example are separated by a lone `%`
line. Strings are printed quoted; integers are printed bare.

## Tiers

### Tier 1 — facts, values, identity (0001–0012)

| # | File | What it exercises |
|---|------|-------------------|
| 0001 | `nullary-fact` | a zero-arity fact and a boolean query |
| 0002 | `string-fact` | a string constant |
| 0003 | `integer-fact` | an integer constant |
| 0004 | `two-facts` | two rows |
| 0005 | `duplicate-facts` | set semantics |
| 0006 | `mixed-types` | `42` and `"42"` are distinct |
| 0007 | `arity-identity` | `state/0`, `state/1`, `state/2` coexist |
| 0008 | `name-collision` | predicate and variable share a spelling |
| 0009 | `empty-string` | the empty string is a value |
| 0010 | `string-escapes` | backslash and quote escapes |
| 0011 | `canonical-zero` | zero is a single canonical value |
| 0012 | `huge-integer` | arbitrary-precision literal |

### Tier 2 — first rules and joins (0013–0030)

| # | File | What it exercises |
|---|------|-------------------|
| 0013 | `copy-rule` | the first rule |
| 0014 | `projection` | drop a column |
| 0015 | `head-constant` | constant in the head |
| 0016 | `repeated-variable` | repeated variable imposes equality |
| 0017 | `two-hop` | two-atom join |
| 0018 | `triangle` | three-atom self-join |
| 0019 | `join-constant` | constant in a body atom |
| 0020 | `nullary-rule` | nullary head and body |
| 0021 | `arity-two-to-one` | project the second column |
| 0022 | `duplicate-body` | repeated body atom |
| 0023 | `three-atom-join` | length-3 chain |
| 0024 | `cross-product` | no shared variable |
| 0025 | `unary-to-binary` | add a constant column |
| 0026 | `binary-to-unary` | project the first column |
| 0027 | `column-permute` | swap columns |
| 0028 | `two-roles` | one relation in two roles |
| 0029 | `head-projects-subset` | head arity below body arity |
| 0030 | `constant-only-head` | ground head guarded by a body |

### Tier 3 — recursion and closure (0031–0050)

| # | File | What it exercises |
|---|------|-------------------|
| 0031 | `parent-ancestor` | transitive closure |
| 0032 | `reachability` | generic reachability |
| 0033 | `reverse-reach` | reachability into a sink |
| 0034 | `cycle-reach` | reachability around a cycle |
| 0035 | `even-odd` | mutual recursion via successor |
| 0036 | `same-generation` | shared-parent join |
| 0037 | `dependency-closure` | dependency closure |
| 0038 | `reverse-dependency` | impact set |
| 0039 | `strongly-connected` | mutual reachability |
| 0040 | `cycle-member` | nodes on a cycle |
| 0041 | `reach-from-seed` | reachability from a seed set |
| 0042 | `common-ancestor` | join of two ancestor relations |
| 0043 | `common-descendant` | join of two descendant relations |
| 0044 | `shared-grandparent` | grandparent join |
| 0045 | `diamond-closure` | diamond dependency |
| 0046 | `component-membership` | two components |
| 0047 | `two-edge-kinds` | closure over united relations |
| 0048 | `two-hop-neighbor` | two-hop neighbor |
| 0049 | `chain-30` | a 30-fact chain |
| 0050 | `star-30` | a 30-fact star |

### Tier 4 — typed and Unicode (0051–0060)

| # | File | What it exercises |
|---|------|-------------------|
| 0051 | `integer-keyed-closure` | integer keys |
| 0052 | `bigint-keyed-closure` | arbitrary-precision keys |
| 0053 | `greek-identifiers` | Greek predicate names |
| 0054 | `cjk-identifiers` | CJK predicate names |
| 0055 | `cyrillic-confusables` | visually similar, distinct spellings |
| 0056 | `escaped-roundtrip` | escaped string constants |
| 0057 | `nfc-distinct-strings` | canonically equivalent strings differ |
| 0058 | `integer-vs-string-join` | integer and string keys differ |
| 0059 | `unicode-constant-join` | Unicode constant match |
| 0060 | `mixed-type-relations` | one relation, mixed kinds |

### Tier 5 — query capabilities (0061–0072)

| # | File | What it exercises |
|---|------|-------------------|
| 0061 | `boolean-query` | ground query, boolean result |
| 0062 | `all-constant-query` | fully ground tuple query |
| 0063 | `repeated-query-variable` | repeated query variable |
| 0064 | `projection-order` | column order from first occurrence |
| 0065 | `multi-atom-query` | query-side join |
| 0066 | `bound-and-free` | bound then free |
| 0067 | `query-derived-only` | query a derived relation |
| 0068 | `query-edb-only` | query the EDB |
| 0069 | `duplicate-results-collapse` | duplicate rows collapse |
| 0070 | `multi-query-document` | several queries in one file |
| 0071 | `empty-derived` | a derived relation with no rows |
| 0072 | `nullary-derived-flag` | derived proposition |

### Tier 6 — idioms and algorithms (0073–0085)

| # | File | What it exercises |
|---|------|-------------------|
| 0073 | `base-plus-recursive` | closure split form |
| 0074 | `left-linear-recursion` | left-linear recursion |
| 0075 | `right-linear-recursion` | right-linear recursion |
| 0076 | `mutual-level` | even/odd over a chain |
| 0077 | `seed-set-reach` | multiple seeds |
| 0078 | `bipartite-membership` | membership join |
| 0079 | `co-publication` | shared-attribute join |
| 0080 | `friends-of-friends` | two-hop social join |
| 0081 | `coauthor-network` | closure over a derived relation |
| 0082 | `role-hierarchy` | transitive role inheritance |
| 0083 | `category-closure` | category ancestry |
| 0084 | `include-closure` | transitive includes |
| 0085 | `call-graph-reach` | call-graph reachability |

### Tier 7 — real-world models (0086–0095)

| # | File | What it exercises |
|---|------|-------------------|
| 0086 | `org-chart` | reporting chain |
| 0087 | `package-deps` | package dependency closure |
| 0088 | `build-prerequisites` | build prerequisite closure |
| 0089 | `network-routes` | route plus backup reachability |
| 0090 | `rbac-roles` | role inheritance |
| 0091 | `supply-chain` | downstream reachability |
| 0092 | `dns-aliases` | alias resolution closure |
| 0093 | `cert-trust` | certificate trust chains |
| 0094 | `course-prerequisites` | prerequisite closure |
| 0095 | `reaching-definitions` | reaching definitions |

### Tier 8 — capstones (0096–0100)

| # | File | What it exercises |
|---|------|-------------------|
| 0096 | `knowledge-graph` | multi-relation joins with Unicode |
| 0097 | `edb-idb-overlap` | asserted and derived tuple appears once |
| 0098 | `rule-replacement` | replacing rules retracts derivations |
| 0099 | `persistence` | reload reproduces the same results |
| 0100 | `grand-finale` | many features, several queries |

## Regenerating

Edit `gen.def`, then:

```sh
sh examples/generate.sh
sh examples/run.sh --bless
```

Always inspect the blessed `.out` diff before committing.
