# Makefile for lab11 (macOS + X11)

CC = gcc
CFLAGS = -I/opt/X11/include -Wall -O2
LDFLAGS = -L/opt/X11/lib -lX11 -lm

MAIN = project
FUNC = projectfunc
OBJS = $(MAIN).o $(FUNC).o gfx3.o
EXEC = project

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(EXEC)

$(FUNC).o: $(FUNC).c $(FUNC).h
	$(CC) $(CFLAGS) -c $(FUNC).c -o $(FUNC).o

$(MAIN).o: $(MAIN).c $(FUNC).h
	$(CC) $(CFLAGS) -c $(MAIN).c -o $(MAIN).o

gfx3.o: gfx3.c gfx3.h
	$(CC) $(CFLAGS) -c gfx3.c -o gfx3.o

clean:
	rm -f *.o $(EXEC)