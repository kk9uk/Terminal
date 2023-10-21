all: read.c exec.c
	gcc read.c exec.c -o Terminal

clean:
	rm Terminal
