# projeto-compilers-cin
Projeto IF688 - Compiladores

## 1. Gerar o compiler
### Basta dar um make na pasta compiler:  
    $ cd compiler  
    $ make

Esse comando irá gerar um compilador que será usado para gerar um arqruivo em IR

## 2. Gerar arquivo IR 
### Ainda dentro da pasta compiler, utilizar o compiler criado para gerar o IR a partir de um arquivo de teste:  
    $ ./compiler test.c


## Referências 
### Pode usar o clang para gerar o arquivo IR e se basear para entender como se dá a conversão:  
    $ clang -S -emit-llvm -O2 test.c   
obs: A flag de otimização pode ser: -O0, -O1, -O2, -O3
