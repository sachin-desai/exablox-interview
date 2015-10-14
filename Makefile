CC = gcc
CFLAGS = -Wall -g -std=gnu99
LFLAGS = -L./lib
INCL = -I./mrkl
LIBS = -lcrypto -lmerkletree

SRC = src
MRKL = mrkl

SRCS = $(wildcard $(SRC)/*.c $(MRKL)/*.c)
OBJS = $(SRCS:.c=.o)

MAIN = exablox

.PHONY = all clean

all: clean $(MAIN)

$(MAIN): $(OBJS)
	@$(CC) $(CFLAGS) $(INCL) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

.c.o:
	@$(CC) $(CFLAGS) $(INCL) -c $< -o $@

clean: 
	@$(RM) $(SRC)/*.o $(MRKL)/*.o *~ $(MAIN)
