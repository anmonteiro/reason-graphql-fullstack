.PHONY: all build run clean test

build:
	dune build

all: build

run:
	dune exec graphql

test:
	dune runtest

clean:
	rm -rf _build *.install
