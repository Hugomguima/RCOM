CC = gcc
FLAGS = -Wall

all: emissor
	
emissor: emissor.c
	$(CC) $(FLAGS) emissor.c llfunctions.c stateMachines.c application.c -o emissor
clean:
	rm -rf emissor
	rm -rf *.exe
	rm -rf *.out
