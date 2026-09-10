CC      = gcc
CFLAGS  = `pkg-config --cflags gtk+-3.0` -Wall -Wextra -O2
LIBS    = `pkg-config --libs gtk+-3.0`
TARGET  = slatetop
SRCS    = $(wildcard src/*.c)
OBJS    = $(SRCS:.c=.o)

PREFIX  = /usr/local
BINDIR  = $(PREFIX)/bin

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

install: $(TARGET)
	install -Dm755 $(TARGET) $(DESTDIR)$(BINDIR)/$(TARGET)

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/$(TARGET)

.PHONY: all clean install uninstall
