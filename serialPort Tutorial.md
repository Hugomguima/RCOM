# RCOM

## TUTORIAL PARA AS PORTAS SERIES VIRTUAIS:

1. abrir um terminal e escrever:

````
sudo socat -d  -d  PTY,link=/dev/ttyS0,mode=777   PTY,link=/dev/ttyS1,mode=777
````

2. abrir mais dois terminais na pasta onde estão os ficheiros

3. 

````
gcc recetor.c llfunctions.c stateMachines.c application.c -o recetor
````


````
gcc emissor.c llfunctions.c stateMachines.c application.c -o emissor
````

4. num terminal escrever:

````
./recetor /dev/ttyS0
````

e no outro:

````
./emissor /dev/ttyS1 pinguim.png
````