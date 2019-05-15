CC     = gcc
CFLAGS = -std=c99 -Wall -Wextra -Wshadow -Wpedantic

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
EXEC = main

#all: $(EXEC)
	#./$(EXEC)

#$(EXEC): $(OBJS)
	#$(CC) $(CFLAGS) $(OBJS) -g -o $(EXEC)

all: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -g -o $(EXEC)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm $(EXEC) $(OBJS)

.PHONY:
	all clean
