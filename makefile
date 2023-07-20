.PHONY: run install

bin/srpn: main.c bin/
	g++ -ggdb3 main.c -o ./bin/srpn

bin/:
	mkdir bin

run: bin/srpn
	./bin/srpn

install: bin/
	echo "Nothing to install"
