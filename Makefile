CC = gcc
CFLAGS = -Wall -Wextra -std=c11 
LIBS = -lGL -lGLU -lglut -lm
TARGET = runes
SRCS = main.c vector.c
BINDIR = $(DESTDIR)$(PREFIX)/bin
MANDIR = $(DESTDIR)$(PREFIX)/share/man/man1
MANPAGE = ${TARGET}.1

.PHONY: all clean style run

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

config:
	mkdir -p $(HOME)/.config
	touch $(HOME)/.config/runes

install: all config
	mkdir -p $(BINDIR)
	cp -f $(TARGET) $(BINDIR)
	chmod 755 $(BINDIR)/$(TARGET)

	mkdir -p $(MANDIR)
	cp -f $(MANPAGE) $(MANDIR)
	chmod 644 $(MANDIR)/$(MANPAGE)

uninstall:
	rm -f $(BINDI)/$(TARGET)
	rm -f $(MANDIR)/$(MANPAGE)

clean:
	rm -f $(TARGET) 

style: 
	find . -name "*.c" -o -name "*.h" | xargs clang-format -i -style=Google

run: $(TARGET)
	./$(TARGET) $(ARGS)
