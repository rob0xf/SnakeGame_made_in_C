CC=ccache clang
CFLAGS=-g1 -Wall -Wextra -Wpedantic -std=c17
LDFLAGS=-lncurses

SOURCES=main.c Snake.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=snake

all:$(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(EXECUTABLE) $(LDFLAGS)

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
	
