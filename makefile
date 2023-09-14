.PHONY: run install remove cleanup test format

CFILES := $(shell find . | grep *.c)

bin/srpn: main.c bin/
	clang -ggdb3 -lm main.c -o ./bin/srpn

bin/:
	mkdir bin

run: bin/srpn
	./bin/srpn

install: bin/srpn
	install bin/srpn ~/.local/bin/srpn

remove: cleanup
	rm ~/.local/bin/srpn

cleanup:
	rm -r ./bin

test: test.lua tests.lua bin/srpn
	echo "Running tests!"
	@lua test.lua

format:
	astyle --style=gnu --indent=spaces=2 $(CFILES)
