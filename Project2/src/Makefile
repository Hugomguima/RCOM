CC = gcc
FLAGS = -Wall

all: download
	
download: main.c
	$(CC) $(FLAGS) main.c arguments_parser.c connection.c -o download
clean:
	rm -rf download
	rm -rf *.exe
	rm -rf *.out
