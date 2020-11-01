CC = gcc
FLAGS = -Wall

all: recetor emissor
	
recetor: recetor.c
	$(CC) $(FLAGS) recetor.c llfunctions.c stateMachines.c application.c -o recetor
emissor: emissor.c
	$(CC) $(FLAGS) emissor.c llfunctions.c stateMachines.c application.c -o emissor
clean:
	rm -rf recetor
	rm -rf emissor
	rm -rf *.exe
	rm -rf *.out
