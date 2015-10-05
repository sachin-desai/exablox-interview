CC = gcc
CFLAGS = -Wall -g
INCLUDES = -I.
LIBS = -lssl -lcrypto
SRCS = main.c send.c recv.c hash.c utils.c
OBJS = $(SRCS:.c=.o)

MAIN = exablox

.PHONY = all depend clean

all: $(MAIN)

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean: 
	$(RM) *.o *~ $(MAIN)
