CC = gcc
FLAGS = -Wall

all: download
	
download: main.c
	$(CC) $(FLAGS) main.c connection.c arguments_parser.c -o download
clean:
	rm -rf download
	rm -rf *.exe
	rm -rf *.out
