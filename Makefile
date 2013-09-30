BIN=gen-test

OBJF = gen_signal.o
SOURCES = main.cpp
CXX= g++
CXXFLAGS+= -Wall -pedantic

all: $(BIN)

$(BIN): $(OBJF) $(SOURCES)
	$(CXX) $(CXXFLAGS) $(OBJF) $(SOURCES) -o $(BIN) 

clean:
	rm -f $(OJBF) $(BIN)

%.o : %.cpp %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@


