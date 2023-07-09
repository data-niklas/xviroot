.POSIX:
.SUFFIXES:
CC     = cc
LIBS= x11 libxine xinerama
CFLAGS = -Wall $(shell pkg-config --cflags $(LIBS)) -lpthread
LDLIBS = $(shell pkg-config --libs $(LIBS))
LDFLAGS =
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
NAME=xviroot

all: build

build: $(SRCS)
	$(CC) -o $(NAME) $^ $(CFLAGS) $(LDLIBS)

debug: $(SRCS)
	$(CC) -o $(NAME) $^ $(CFLAGS) -g $(LDLIBS)

test:
	$(CC) $(LDFLAGS) -o $(NAME) $(wildcard *.o) $(LDLIBS)

clean:
	rm -f $(wildcard *.o) $(NAME)
