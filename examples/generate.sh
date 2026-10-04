#!/bin/sh -eu
# generate.sh - expand gen.def into the per-example source files.
#
# gen.def records each example as a sequence of sections:
#
#   @@ NNNN-name
#   ! program      -> NNNN-name.dlx   facts and rules
#   ! queries      -> NNNN-name.dlq   one ?- query per line
#   ! a / ! b      -> NNNN-name.dlxa/.dlxb   program variants
#   ! steps        -> NNNN-name.steps scripted lifecycle
#
# The generated files are checked in and are what run.sh executes. Run
# generate.sh only after editing gen.def; then re-bless the goldens with
# run.sh --bless.

dir=$(cd -- "$(dirname -- "$0")" && pwd)
def=$dir/gen.def
name=
target=

while IFS= read -r line || [ -n "$line" ]; do
    case $line in
        "@@ "*)
            name=${line#@@ }
            target=
            ;;
        "! "*)
            section=${line#! }
            case $section in
                program) target=$dir/$name.dlx ;;
                queries) target=$dir/$name.dlq ;;
                a) target=$dir/$name.a ;;
                b) target=$dir/$name.b ;;
                steps) target=$dir/$name.steps ;;
                *)
                    echo "generate: unknown section: $section" >&2
                    exit 1
                    ;;
            esac
            : > "$target"
            ;;
        *)
            if [ -n "$target" ]; then
                printf '%s\n' "$line" >> "$target"
            fi
            ;;
    esac
done < "$def"

echo "generate: ok"
