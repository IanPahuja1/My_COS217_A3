# Assignment 3 Makefile

# Compiler and flags
GCC = gcc217
CFLAGS = -std=c90 -Wall -pedantic -O2

# Targets
all: testsymtablelist testsymtablehash

# Executable for linked list implementation
testsymtablelist: testsymtable.o symtablelist.o
	$(GCC) $(CFLAGS) -o testsymtablelist testsymtable.o symtablelist.o

# Executable for hash table implementation
testsymtablehash: testsymtable.o symtablehash.o
	$(GCC) $(CFLAGS) -o testsymtablehash testsymtable.o symtablehash.o

# Object file rules
testsymtable.o: testsymtable.c symtable.h
	$(GCC) $(CFLAGS) -c testsymtable.c

symtablelist.o: symtablelist.c symtable.h
	$(GCC) $(CFLAGS) -c symtablelist.c

symtablehash.o: symtablehash.c symtable.h
	$(GCC) $(CFLAGS) -c symtablehash.c

# Clean up build artifacts
clean:
	rm -f *.o testsymtablelist testsymtablehash
