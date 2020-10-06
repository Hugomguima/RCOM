# RCOM

## TUTORIAL PARA AS PORTAS SERIES VIRTUAIS:

1. abrir um terminal e escrever:

````
sudo socat -d  -d  PTY,link=/dev/ttyS0,mode=777   PTY,link=/dev/ttyS1,mode=777
````

2. abrir mais dois terminais na pasta onde estão os ficheiros

3. 

````
gcc read.c -o read
````


````
gcc write.c -o write
````

4. num terminal escrever:

````
./read /dev/ttyS0
````

e no outro:

````
./write /dev/ttyS1
````