#################################################
# Makefile
#################################################
# directorys
SRCDIR=src

# compiler
CC=cc

# compile and link option(both)
DEBUG=-O0
BIN=a.out
OBJS=$(SRCDIR)/sock_client.o

# compile option
LIBS=

# link option
CFLAGS=
LINKOPT= 

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
