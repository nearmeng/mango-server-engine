CXX=g++
CXXFLAGS= -g -Wall -fPIC

BIN=libxreplacer.a
SRC=$(wildcard *.cpp)
OBJ=$(SRC:.cpp=.o)
DEP=$(SRC:.cpp=.d)

.PHONY: all clean test

all: $(BIN)
clean: 
	@rm -rfv $(BIN) $(OBJ) $(DEP) test

$(BIN): $(OBJ) $(DEP)
	@ar rv $@ $^

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

%.d: %.cpp
	$(CXX) -MM $(CXXFLAGS) $< -o $@

ifneq ($(MAKECMDGOALS),clean)
    -include $(DEP)
endif
