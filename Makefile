CC = gcc
CFLAGS = -Wall -std=gnu99
LFLAGS = -L./lib
INCL = -I./mrkl
LIBS = -lcrypto -lmerkletree

MAIN = src
MRKL = mrkl

SRCS = $(wildcard $(MAIN)/*.c $(MRKL)/*.c)
OBJS = $(SRCS:%.c=%.o)


BIN = exablox

.PHONY = all clean
all: clean $(BIN)

debug: CFLAGS += -ggdb
debug: clean $(BIN)

release: CFLAGS += -O3 -fgnu89-inline -Wno-error=unused-function
release: clean $(BIN)

$(BIN): $(OBJS)
	@$(CC) $(CFLAGS) $(INCL) -o $(BIN) $(OBJS) $(LFLAGS) $(LIBS)

.c.o:
	@$(CC) $(CFLAGS) $(INCL) -c $< -o $@

clean: 
	@$(RM) $(MAIN)/*.o $(MRKL)/*.o *~ $(BIN)
