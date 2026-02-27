#!/bin/bash

echo "=== Generating parser ==="
yacc -d -y --verbose 22101459_22101458.y

echo "=== Compiling parser ==="
g++ -w -c -o y.o y.tab.c

echo "=== Generating scanner ==="
flex 22101459_22101458.l

echo "=== Compiling scanner ==="
g++ -fpermissive -w -c -o l.o lex.yy.c

echo "=== Linking ==="
g++ y.o l.o -o a.exe

echo "=== Running on input: ${1:-input.c} ==="
./a.exe ${1:-input.c}

echo "=== Output log ==="
cat 22101459_22101458_log.txt
