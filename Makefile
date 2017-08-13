# Makefile
CC = gcc
CFLAGS = -g -Wall
CLIBS = -pthread
OBJS_SERV = hash.o str.o server.o wrapunix.o readline.o daemon.o
OBJS_CLI = client.o wrapunix.o
SERV = server
CLI = client
BIN_DIR = bin

.SUFFIXES: .c .o

$(BIN_DIR) : $(SERV) $(CLI)

$(SERV): $(OBJS_SERV)
	$(CC) -o $(SERV) $(CLIBS) $(CFLAGS) $(OBJS_SERV)

$(CLI): $(OBJS_CLI)
	$(CC) -o $(CLI) $(CLIBS) $(CFLAGS) $(OBJS_CLI)

.c.o:
	$(CC) -c $(CFLAGS) $<

.PHONY: clean
clean:
	$(RM) $(SERV) $(CLI) $(OBJS_CLI) $(OBJS_SERV)

client.o: wrapunix.h
