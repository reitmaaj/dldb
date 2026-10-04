#!/bin/sh -eu
# run.sh [--bless]
#
# Run every dldb example through the CLI and compare its normalized output to
# the checked-in .out file. With --bless, (re)write the .out files instead.
#
# Output normalization: within each query result the column-name header (or a
# single true/false boolean) is kept in place and the remaining data rows are
# sorted, because dldb query row order carries no guarantee. Results of the
# queries in one example are separated by a lone '%' line.

HERE=$(cd -- "$(dirname -- "$0")" && pwd)
DIR=$HERE
BIN=${DLDB:-"$HERE/../build/dldb"}
TMPD=$(mktemp -d)
DB=$TMPD/db
BLESS=0
if [ "${1:-}" = "--bless" ]; then
    BLESS=1
fi

cleanup() {
    rm -rf "$TMPD"
}
trap cleanup EXIT

normalize() {
    tmp=$TMPD/norm
    cat > "$tmp"
    hdr=$(sed -n '1p' "$tmp")
    if [ "$hdr" = "true" ] || [ "$hdr" = "false" ]; then
        cat "$tmp"
    else
        printf '%s\n' "$hdr"
        sed -n '2,$p' "$tmp" | LC_ALL=C sort
    fi
    rm -f "$tmp"
}

append_result() {
    norm=$1
    if [ "$count" -eq 0 ]; then
        result=$norm
    else
        result=$result'
%
'"$norm"
    fi
    count=$((count + 1))
}

run_default() {
    base=$1
    rm -f "$DB"
    "$BIN" init "$DB" > /dev/null
    "$BIN" load "$DB" "$base.dlx" > /dev/null
    "$BIN" check --full "$DB" > /dev/null
    count=0
    result=
    while IFS= read -r line || [ -n "$line" ]; do
        [ -n "$line" ] || continue
        raw=$("$BIN" query "$DB" "$line")
        norm=$(printf '%s' "$raw" | normalize)
        append_result "$norm"
    done < "$base.dlq"
    printf '%s' "$result"
}

run_steps() {
    base=$1
    rm -f "$DB"
    count=0
    result=
    while IFS= read -r line || [ -n "$line" ]; do
        [ -n "$line" ] || continue
        verb=${line%% *}
        arg=${line#* }
        if [ "$verb" = "$line" ]; then
            arg=
        fi
        case "$verb" in
            init)
                "$BIN" init "$DB" > /dev/null
                ;;
            load)
                "$BIN" load "$DB" "$base.$arg" > /dev/null
                ;;
            replace)
                "$BIN" replace "$DB" "$base.$arg" > /dev/null
                ;;
            reload)
                rm -f "$DB"
                "$BIN" init "$DB" > /dev/null
                "$BIN" load "$DB" "$base.$arg" > /dev/null
                ;;
            reopen)
                :
                ;;
            query)
                raw=$("$BIN" query "$DB" "$arg")
                norm=$(printf '%s' "$raw" | normalize)
                append_result "$norm"
                ;;
            dump)
                raw=$("$BIN" dump "$DB")
                norm=$(printf '%s' "$raw" | normalize)
                append_result "$norm"
                ;;
            *)
                echo "examples: unknown step: $line" >&2
                return 1
                ;;
        esac
    done < "$base.steps"
    printf '%s' "$result"
}

check_one() {
    base=$1
    actual=$2
    out=$base.out
    if [ "$BLESS" -eq 1 ]; then
        printf '%s\n' "$actual" > "$out"
        return 0
    fi
    if [ ! -e "$out" ]; then
        echo "examples: missing golden: $out" >&2
        return 1
    fi
    expected=$(cat "$out")
    if [ "$actual" != "$expected" ]; then
        echo "examples: mismatch: $base" >&2
        return 1
    fi
    return 0
}

fail=0
for dlx in "$DIR"/[0-9][0-9][0-9][0-9]-*.dlx; do
    [ -e "$dlx" ] || continue
    base=${dlx%.dlx}
    actual=$(run_default "$base")
    check_one "$base" "$actual" || fail=1
done
for steps in "$DIR"/[0-9][0-9][0-9][0-9]-*.steps; do
    [ -e "$steps" ] || continue
    base=${steps%.steps}
    actual=$(run_steps "$base")
    check_one "$base" "$actual" || fail=1
done

if [ "$fail" -ne 0 ]; then
    echo "examples: FAILED" >&2
    exit 1
fi
echo "examples: ok"
