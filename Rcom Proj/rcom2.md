# Projeto 2 Rcom

gnu22E0 -> switch 5
gnu23E0 -> Switch 1
gnu24E0 -> Swtich 6
gnu24E1 -> Switch 7





### tux3

IP : 172.16.20.1/24
MAC : 00:21:5a:5a:78:c7

### tux4

IP : 172.16.20.254/24
MAC : 00:22:64:a7:26:a2

tux4 eth1

IP : 172.16.21.253/24
MAC : 00:e0:7d:c8:7c:55


### tux2

IP : 172.16.21.1/24
MAC : 00:21:5a:5a:76:a8

## Experiencia 2

Os logs foram obtidos através dos ficheiros guardaos

O ping ao pc2 não recebe pois não há ua ligação.


## Experiencia 3

Logs também já foram guardados


> Quer na experiencia 2 quer na 3 usei o guia do Daniel depois podemos usar isso para explicar passos restantes, que eu percebi bem as coisas





# AUla final novas portas

gnu23E0 -> Switch 1
gnu22E0 -> Switch 2
gnu24E0 -> Swtich 3
gnu24E1 -> Switch 4

Router -> switch 5

NAT -> 1

Trocar do switch oara o router e dar login no gtk terminal



### tux3

IP : 172.16.20.1/24
MAC : 00:21:5a:5a:78:c7

### tux4

IP : 172.16.20.254/24
MAC : 00:22:64:a7:26:a2

tux4 eth1

IP : 172.16.21.253/24
MAC : 00:e0:7d:c8:7c:55


### tux2

IP : 172.16.21.1/24
MAC : 00:21:5a:5a:76:a8


# Ultima tentativa

### Ligar Portas (foto no telemóvel da Ultima aula)

TUX23E0    -> Switch Porta 1
TUX22E0    -> Switch Porta 2
TUX24E0    -> Switch Porta 3
TUX24E1    -> Switch Porta 4
ROUTERGE0  -> Switch Porta 5      (router-switch)
ROUTERGE1  -> Prateleira Porta 1  (router-lab network)


### Configurar IPS


#### Ip do Router

Configurar o IP do router
```
»interface fastethernet 0/0                 (0/0 - deve ser bloco 0, porta 0 do router)
»ip address 172.16.21.254    255.255.255.0     (deve ser endereço e máscara aka /24 )
»no shutdown
»exit
»show interface gigabitethernet 0/0

```

dar um Ip ao router para poder ligar ás no porta GE0, falta dar um ip para comunicação exterior a partir da porta GE1:

```
»interfacefastethernet 0/1                 (0/0 - deve ser bloco 0, porta 0 do router)
»ip address 172.16.1.29    255.255.255.0       (deve ser endereço e máscara aka /24 )
»no shutdown
»exit
»show interface gigabitethernet 0/1  
```

Aproveitar que se está na consola da switch 
```
»ip route 0.0.0.0 0.0.0.0 172.16.1.254
»ip route 172.16.20.0 255.255.255.0 172.16.21.253
```
