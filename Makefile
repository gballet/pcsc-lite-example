%.o: %.c
	gcc -c $< -o $@ `pkg-config libpcsclite --cflags`

all: main.o
	gcc $< -o testsc `pkg-config libpcsclite --libs`
