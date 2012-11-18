SOURCES = scope.c text_file.c
OBJECTS = $(SOURCES:.c=.o)

CC += -Wall -std=gnu99 -g

%.o: %.c %.h
	$(CC) -c $< -o $@ `pkg-config --cflags uchardet`

all: $(OBJECTS)
	$(CC) $(OBJECTS) -o scope `pkg-config --libs uchardet`

install:
	@echo "Can't install this project"

uninstall:
	@echo "we can't uninstall this"

clean:
	rm -f *.o
	rm -f scope
