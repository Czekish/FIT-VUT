CC=gcc
CFLAGS=-std=c99 -Wall -pedantic -Wextra -g
OBJFILES=main.o parser.o scanner.o str.o symtable.o expressions.o stack.o generate.o
TARGET=ifj

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) -lm

clean:
	rm -f $(OBJFILES) $(TARGET) *~

test:
	python3 tests/run_tests.py

