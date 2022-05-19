
CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -Wno-unknown-pragmas -Wno-unused-parameter -g3

EXE = .exe
ARCHIVE = xhosta05.zip

RELEASE = ifj19
PROGS = ifj19 test_string test_scanner test_codeGenerator
INTERPRET = ic19int

SOURCE = main.c parser.c parserExp.c scanner.c codeGenerator.c token.c dynamicString.c error.c stack.c stackToken.c stringStack.c symtable.c
HEADER = parser.h scanner.h codeGenerator.h token.h dynamicString.h error.h stack.h stackToken.h stringStack.h symtable.h global_var.h

IN_TEST_FILE = test.test
COMP_TEST_FILE = comp.test
OUT_TEST_FILE = out.test
EXPECTED_TEST_FILE = expected.test

.PHONY: test test_string test_scanner clean run zip

release: $(RELEASE)

all: $(PROGS)

zip: $(SOURCE) $(HEADER) Makefile
	zip $(ARCHIVE) $(SOURCE) $(HEADER) Makefile

run: $(RELEASE)
	./$(RELEASE) < $(IN_TEST_FILE)

ifj19: main.o parser.o parserExp.o scanner.o codeGenerator.o token.o dynamicString.o error.o stack.o stackToken.o stringStack.o symtable.o
	$(CC) $(CFLAGS) $^ -o $@

## main ##
main.o: main.c
	$(CC) $(CFLAGS) -c $^ -o $@

## Parser ##
parser: parser.o scanner.o codeGenerator.o stringStack.o
	-$(CC) $(CFLAGS) $^ -o $@

parser.o: parser.c
	-$(CC) $(CFLAGS) -c $^ -o $@

## Parser Expressions##
parserExp: parserExp.o scanner.o codeGenerator.o
	$(CC) $(CFLAGS) $^ -o $@

parserExp.o: parserExp.c
	$(CC) $(CFLAGS) -c $^ -o $@


## Scanner ##
scanner: scanner.o token.o dynamicString.o
	$(CC) $(CFLAGS) $^ -o $@

scanner.o: scanner.c
	$(CC) $(CFLAGS) -c $^ -o $@

## Code Generator ##
codeGenerator.o: codeGenerator.c
	$(CC) $(CFLAGS) -c $^ -o $@

## Other modules ##
token.o: token.c
	$(CC) $(CFLAGS) -c $^ -o $@

dynamicString.o: dynamicString.c
	$(CC) $(CFLAGS) -c $^ -o $@

stack.o: stack.c
	$(CC) $(CFLAGS) -c $^ -o $@

stackToken.o: stackToken.c
	$(CC) $(CFLAGS) -c $^ -o $@

stringStack.o: stringStack.c
	$(CC) $(CFLAGS) -c $^ -o $@

error.o: error.c
	$(CC) $(CFLAGS) -c $^ -o $@

symtable.o: symtable.c
	$(CC) $(CFLAGS) -c $^ -o $@

##//END Scanner ##

## Testing ##

test: $(RELEASE) $(INTERPRET) $(IN_TEST_FILE) $(EXPECTED_TEST_FILE)
	./$(RELEASE) <$(IN_TEST_FILE) >$(COMP_TEST_FILE)
	./$(INTERPRET) $(COMP_TEST_FILE) >$(OUT_TEST_FILE)
	diff -q $(OUT_TEST_FILE) $(EXPECTED_TEST_FILE)


test_string: test_string.c dynamicString.o error.o
	$(CC) $(CFLAGS) -Wno-unused-parameter $^ -o $@
	./test_string

test_scanner: scanner
	./scanner$(EXE) $(IN_TEST_FILE)

test_codeGenerator: test_codeGenerator.c codeGenerator.o dynamicString.o error.o
	$(CC) $(CFLAGS) $^ -o $@

test_hex: testHex.c
	$(CC) $(CFLAGS) $^ -o $@
	./$@

#CLEAN
clean:
	rm *.o *.exe $(ARCHIVE) $(PROGS)

unsafe_clean_will_delete_source:
	rm *.o *.exe *.c *.h $(ARCHIVE) $(PROGS) Makefile
