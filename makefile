.PHONY: run install remove cleanup test

bin/srpn: main.c bin/
	gcc -ggdb3 -lm main.c -o ./bin/srpn

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
