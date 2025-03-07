CC = gcc
CFLAGS = -Wall -Wextra -std=c11 
LIBS = -lGL -lGLU -lglut -lm
TARGET = mouse_gestures
SRCS = main.c vector.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)
