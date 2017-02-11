CXX=clang++
CXXFLAGS=-g -std=c++11 -Wall -pedantic -I./lua/src -I/usr/local/opt/openssl/include
BIN=server

SRC=$(wildcard *.cc)
OBJ=$(SRC:%.cc=%.o)

all: $(OBJ)
	$(CXX) -o $(BIN) -lm \
		-L./lua/src -llua \
		-L/usr/local/opt/openssl/lib -lcrypto \
		-L/usr/local/opt/boost/lib -lboost_system \
		$^

%.o: %.c
	$(CXX) $@ -c $<

clean:
	rm -f *.o
	rm $(BIN)


