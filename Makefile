all: read.c exec.c
	gcc main.c shell.c -o Terminal

clean:
	rm Terminal
