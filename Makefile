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
run: clean build
	./$(NAME) "/usr/share/Komorebi/yasuo/yasuo.mp4" "/usr/share/Komorebi/kobelbass/kobelbass.mp4" loop -v 20 screen 1
	

build: $(SRCS)
	$(CC) -o $(NAME) $^ $(CFLAGS) $(LDLIBS)	

debug: $(SRCS)
	$(CC) -o $(NAME) $^ $(CFLAGS) -g $(LDLIBS)

test:
	$(CC) $(LDFLAGS) -o $(NAME) $(wildcard *.o) $(LDLIBS)

clean:
	rm -f $(wildcard *.o) $(NAME)