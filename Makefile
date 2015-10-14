CC = gcc
CFLAGS = -Wall -g -std=gnu99
LFLAGS = -L./lib
INCL = -I./merkle
LIBS = -lcrypto -lmerkletree

SRC = src
MERKLE = merkle

SRCS = $(wildcard $(SRC)/*.c $(MERKLE)/*.c)
OBJS = $(SRCS:.c=.o)

MAIN = exablox

.PHONY = all clean

all: clean $(MAIN)

$(MAIN): $(OBJS)
	@$(CC) $(CFLAGS) $(INCL) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

.c.o:
	@$(CC) $(CFLAGS) $(INCL) -c $< -o $@

clean: 
	@$(RM) $(SRC)/*.o $(MERKLE)/*.o *~ $(MAIN)
