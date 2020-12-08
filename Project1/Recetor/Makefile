CC = gcc
FLAGS = -Wall

all: recetor
	
recetor: recetor.c
	$(CC) $(FLAGS) recetor.c llfunctions.c stateMachines.c application.c -o recetor
clean:
	rm -rf recetor
	rm -rf *.exe
	rm -rf *.out
	rm -rf *.png
	rm -rf *.jpeg
	rm -rf *.txt