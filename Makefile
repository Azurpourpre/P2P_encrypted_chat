CC := g++
CFLAGS := -Wall -Wextra -g -static
LDFLAGS := -L lib/cryptopp -lcryptopp

build_and_run: build
	./SecChat

build: make_cryptopp
	$(CC) main.cpp $(CFLAGS) -o SecChat $(LDFLAGS)

make_cryptopp:
	make -C lib/cryptopp static
