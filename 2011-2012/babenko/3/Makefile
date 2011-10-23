CC=g++
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=solution.cpp
OBJECTS=$(SOURCES:.cpp=.o)

EXECUTABLE=solution

all: $(SOURCES) \
    solution

solution: $(OBJECTS) \
    $(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o: \
    $(CC) $(CFLAGS) $< -o $@


