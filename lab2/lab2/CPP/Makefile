CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

STRUCT = cpp/array.cpp cpp/avl.cpp cpp/dlist.cpp cpp/flist.cpp cpp/queue.cpp cpp/stack.cpp cpp/set.cpp
HDR = $(wildcard headers/*.h) $(wildcard database/*.h)

PROGRAMS = dbms Brackets Turtles Histogram Huffman HashBench Roman Lfu

all: $(PROGRAMS)

# Задание 2: интерфейс ЛР1 с командами множества
dbms: main.cpp database/database.cpp $(STRUCT) $(HDR)
	$(CXX) $(CXXFLAGS) main.cpp database/database.cpp $(STRUCT) -o $@

Huffman: tasks/Huffman.cpp cpp/huffman.cpp $(STRUCT) $(HDR)
	$(CXX) $(CXXFLAGS) tasks/Huffman.cpp cpp/huffman.cpp $(STRUCT) -o $@

Lfu: tasks/Lfu.cpp cpp/lfu.cpp $(STRUCT) $(HDR)
	$(CXX) $(CXXFLAGS) tasks/Lfu.cpp cpp/lfu.cpp $(STRUCT) -o $@

%: tasks/%.cpp $(STRUCT) $(HDR)
	$(CXX) $(CXXFLAGS) $< $(STRUCT) -o $@

clean:
	rm -f $(PROGRAMS)

.PHONY: all clean
