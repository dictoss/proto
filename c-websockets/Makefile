#################################################
# Makefile
#################################################
# directorys
SRCDIR=src

# compiler
CC=cc

# compile and link option(both)
DEBUG=-Wall -O0 -g
BIN=a.out
OBJS=$(SRCDIR)/wsmain.o

# compile option
LIBS=

# link option
CFLAGS=
LDFLAGS=-lwebsockets -ljansson

#################################################
#
################################################
.PHONY:all
all:$(BIN)

$(BIN):$(OBJS)
	$(CC) $(DEBUG) $(LDFLAGS) -o $(BIN) $(LIBS) $(OBJS)

.PHONY:clean
clean:
	rm -f $(BIN) $(OBJS) *~

#################################################
# suffixes
#################################################
.SUFFIXES: .c .o
.c.o:
	$(CC) $(DEBUG) $(CFLAGS) -c $< -o $@
