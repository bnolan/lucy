CXX=clang++
CXXFLAGS=-g -std=c++11 -Wall -pedantic -I../lua/src
BIN=server

SRC=$(wildcard *.cc)
OBJ=$(SRC:%.cc=%.o)

all: $(OBJ)
	$(CXX) -o $(BIN) -L../lua/src -llua -lm $^

%.o: %.c
	$(CXX) $@ -c $<

clean:
	rm -f *.o
	rm $(BIN)


