#!/bin/bash

# Build and run 22101459_22101458 Compiler Lab 2

echo "=== Generating parser ==="
yacc -d -y --verbose 22101459_22101458.y
echo "Generated y.tab.c / y.tab.h"

echo "=== Compiling parser ==="
g++ -w -c -o y.o y.tab.c

echo "=== Generating scanner ==="
flex 22101459_22101458.l
echo "Generated lex.yy.c"

echo "=== Compiling scanner ==="
g++ -fpermissive -w -c -o l.o lex.yy.c

echo "=== Linking ==="
g++ y.o l.o -o a.exe

echo "=== Running on input: ${1:-input.c} ==="
./a.exe ${1:-input.c}

echo "=== Output log ==="
cat 22101459_22101458_log.txt
