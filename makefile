
build:
	g++ src/main.cpp -g -Wall -o main
clean:
	rm -f main test

test: clean
	g++ test.cpp -o test && ./test

