CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

SRC = main.cpp $(wildcard database/*.cpp) $(wildcard cpp/*.cpp)
HDR = $(wildcard headers/*.h) $(wildcard database/*.h)

dbms: $(SRC) $(HDR)
	$(CXX) $(CXXFLAGS) $(SRC) -o dbms

clean:
	rm -f dbms
