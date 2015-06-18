
OBJ := $(patsubst %.cpp,%.o,$(wildcard *.cpp))
CXXFLAGS := -Wall -O2 -g -std=c++11


all: parse

parse: $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	$(RM) $(OBJ) parse
