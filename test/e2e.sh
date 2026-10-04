#!/bin/sh -eu

BIN=build/dldb
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT

"$BIN" init "$TMP/db"

printf '%s\n' \
    'parent("alice", "bob").' \
    'parent("bob", "carol").' \
    'ancestor(X, Y) :- parent(X, Y).' \
    'ancestor(X, Z) :- ancestor(X, Y), parent(Y, Z).' \
    'n(42).' \
    'n("42").' > "$TMP/program.dlx"

"$BIN" load "$TMP/db" "$TMP/program.dlx"

out=$("$BIN" query "$TMP/db" '?- ancestor("alice", X).')
printf '%s\n' "$out" | grep -q 'bob'
printf '%s\n' "$out" | grep -q 'carol'

out=$("$BIN" query "$TMP/db" '?- n(X).')
printf '%s\n' "$out" | grep -q '42'

if "$BIN" query "$TMP/db" '?- n(43).' | grep -q true; then
    echo "e2e: unexpected true for n(43)" >&2
    exit 1
fi

"$BIN" check --full "$TMP/db" > /dev/null

"$BIN" dump "$TMP/db" | grep -q 'ancestor(X, Z) :- ancestor(X, Y), parent(Y, Z).'

echo "e2e: ok"
