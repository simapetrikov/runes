CC = gcc
CFLAGS = -Wall -Wextra -std=c11 
LIBS = -lGL -lGLU -lglut -lm
TARGET = mouse_gestures
SRCS = main.c vector.c

.PHONY: all clean style run

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

clean:
	rm -f $(TARGET) 

style: 
	find . -name "*.c" -o -name "*.h" | xargs clang-format -i -style=Google

run: $(TARGET)
	./$(TARGET) $(ARGS)
