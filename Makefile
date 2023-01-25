CC=cc
CFLAGS=-Wall -O2 `sdl2-config --cflags`
LFLAGS=-s `sdl2-config --libs` -lm

OBJS=main.o

TARGET=main

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -fv -- $(OBJS) $(TARGET)
