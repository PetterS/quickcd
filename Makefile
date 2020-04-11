CC = gcc
CXX = g++

all: quickcd-store

quickcd-store: quickcd-store.o third-party/sqlite3.o
	$(CXX) quickcd-store.o third-party/sqlite3.o -lpthread -ldl -o quickcd-store

%.o: %.cpp $(DEPS) Makefile
	$(CXX) -c -Wall -Werror -O3 -o $@ $<

%.o: %.c $(DEPS) Makefile
	$(CC) -c -O3 -o $@ $<

format:
	clang-format -i *.cpp
	python3 -m black .

clean:
	rm -rf **/*.o
