set shell := ["sh", "-eu", "-c"]

CC := env_var_or_default("CC", "cc")
STRICT := "-std=c89 -pedantic-errors -Wall -Wextra -Werror -O2"
INCS := "-I../libdldb89/include -I../libdlx89/include -I../libdlq89/include"
LIBS := "../libdldb89/build/libdldb89.a ../libdlx89/build/libdlx89.a ../libdlq89/build/libdlq89.a ../libdatalog89/build/libdatalog89.a ../libunicode89/build/libunicode89.a -lsqlite3"

default: build

deps:
	cd ../libdldb89 && just build

build: deps
	mkdir -p build
	{{CC}} {{STRICT}} {{INCS}} -o build/dldb cmd/dldb/main.c {{LIBS}}

# Run the graded example corpus and compare to the checked-in goldens.
examples: build
	sh examples/run.sh

# Regenerate the example goldens after changing the corpus.
bless: build
	sh examples/run.sh --bless

test: build
	sh test/e2e.sh
	sh examples/run.sh

clean:
	rm -rf build
