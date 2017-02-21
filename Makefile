CXX=clang++
CXXFLAGS=-g -std=c++11 -Wall -pedantic -I./lua/src -I/usr/local/opt/openssl/include
BIN=server

SRC=$(wildcard *.cc) $(wildcard vendor/*.cc)
OBJ=$(SRC:%.cc=%.o)

all: $(OBJ)
	$(CXX) -o $(BIN) -lm \
		-L./lua/src -llua \
		-L/usr/local/opt/openssl/lib -lcrypto \
		-L/usr/local/opt/boost/lib -lboost_system \
		$^

%.o: %.c
	$(CXX) $@ -c $<

bootstrap:
	curl -O https://www.lua.org/ftp/lua-5.3.4.tar.gz
	tar xvfz lua-5.3.4.tar.gz
	mv lua-5.3.4 lua
	cd lua && make linux

clean:
	rm -f *.o
	rm $(BIN)
	rm -rf lua
	rm -f lua-5.3.4.tar.gz


