CC := g++
LDFLAGS := -L lib/cryptopp -lcryptopp

build_and_run: build
	./SecChat

build: #make_cryptopp
	$(CC) main.cpp -o SecChat $(LDFLAGS)

make_cryptopp:
	make -C lib/cryptopp