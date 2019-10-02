CFLAGS=--std=c11 -Wall -Werror -Wpedantic -Wextra -Wconversion -Wunused-macros
HEADERS=$(wildcard src/*.h)
OBJECTS=$(patsubst %.c, %.o, $(wildcard src/*.c))

.PHONY: all
all: servant

.PHONY: run
run: all
	./servant

.PHONY: clean
clean:
	rm $(OBJECTS) ./servant

servant: $(OBJECTS)
	$(CC) -o $@ $^

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<
