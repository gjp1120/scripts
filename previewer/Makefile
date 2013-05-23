SOURCES = scope.c text_file.c filetype.c
OBJECTS = $(SOURCES:.c=.o)

#CC += -Wall -std=gnu99 -g -lefence
CC += -Wall -std=gnu99 -g

%.o: %.c %.h
	$(CC) -c $< -o $@ `pkg-config --cflags uchardet`

all: $(OBJECTS)
	$(CC) $(OBJECTS) -o scope `pkg-config --libs uchardet`

install:
	cp scope ~/.bin

uninstall:
	@echo "we can't uninstall this"

clean:
	rm -f *.o
	rm -f scope
