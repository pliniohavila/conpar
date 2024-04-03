# Concorrência e Paralelismo 

Repositório destinado aos meus estudos introdutórios de concorrência e paralelismo na linguagem C. 

## O que são threads?


Para este estudo estarei utilizando a Linguagem C e a biblioteca `POSIX threads`. Que segundo a sua documentação (`man pthreads`): 
>   POSIX.1  specifies a set of interfaces (functions, header files) for threaded programming commonly known as POSIX threads, or Pthreads.  A single
    process can contain multiple threads, all of which are executing the same program.  These threads share the same global  memory  (data  and  heap
    segments), but each thread has its own stack (automatic variables).

Para utilizar, adicionamos a flag `-pthread` ao compilar os nossos códigos. 


[COLOCAR UM CÓDIGO SIMPLES DE UMA THREAD]



[EXPLICAÇÃO SORBE AS FUNÇÕES UTILIZADAS]


##  O que é Concorrência? 

## O que é Paralelismo?

# Sincronização de threads 

## Race Conditions

[falar sobre o problema de *race conditions*]

Como vimos acima sobre `threads`, que diferente de processos, `threads` compartilham o mesmos recursos disponíveis para o processo a qual pertencem. Nesse cenário, pode acontecer comportamentos inesperados quando `threads` acessam um mesmo recursos ao mesmo tempo ou mesma região da memória. 
[#######]
A esses casos são denominados *race conditions*. 
Podemos dizer que está ocorrendo uma *race condition* quando 

 A contention or race condition often occurs when two or more threads need to perform operations on the same memory area, but the results of computations depends on the order in which these operations are performed. Mutexes are used for serializing shared resources such as memory. 

Vejamos o exemplo. O código abaixo, temos uma variável global `count` que será incrementada `n` vezes por 5 `threads` "ao mesmo tempo". 

```c 
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

volatile unsigned int count = 0;
void    *increment(void*);

int     main(int argc, char **argv)
{
    pthread_t       threads[5];
    int             i;
    int             qtd;

    if (argc < 2)
        qtd = 10000;
    else 
        qtd = atoi(argv[1]);
    i = 0;
    while (i < 5) 
    {
        pthread_create(&threads[i], NULL, increment, (void*)&qtd);
        i++;
    }
    i = 0;
    while (i < 5) 
    {
        pthread_join(threads[i], NULL);
        i++;
    }
    printf("Count: %d\n", count);
    return (0);
}

void    *increment(void *ptr)
{
    int         i;
    int         qtd;

    qtd = *(int*)ptr;
    i = 0;
    while(i++ < qtd)
        count++;
    return ((void*)0);
}
```

Executando o código com valores mais altos podemos ver que os resultados não saem com esperado. 

```shell 
❯ make race
gcc -Wall -Wextra -Werror -pthread race.c -o race 
❯ ./race
Count: 44599
❯ ./race 10
Count: 50
❯ ./race 100
Count: 500
❯ ./race 1000
Count: 5000
❯ ./race 10000
Count: 26448
❯ ./race 100000
Count: 150643
❯ ./race 1000000
Count: 1506256
```

[#######]
Aproveitar e colocar uma nota sobre a palavra chave `volatile`. 


Diante desse cenários que temos a **sincronização de *threads***. 

## Mecanismos para lidar com sincronização de `threads`

A biblioteca `POSIX threads` fornece três mecanismo para lidar com sincronização de `threads`. São: 

1. Mutexes (Mutual exclusion lock): Bloqueia o acesso a determinada variável/recurso para outras `threads`.
2. Joins: Faz com que uma `thread` aguarde que outras estejam terminadas. 
3. Condition Variables: Utiliza o tipo `pthread_cond_t` para controlar o acesso a determinada variável/recurso.

### Mutexes 

*Mutexes* são utilizados para evitar comportamentos inesperados ou inconsistências com os dados durante operações de várias `threads`na mesma área de memória ou recurso. É uma forma de prevenir *race conditions*. 

O seu uso se dá por meio de uma variável do tipo `pthread_mutex_t`. Quando uma `thread` tentar acessar uma área de memória, primeiro ela tentar bloquear a variável *mutex*; caso consiga: realiza sua ação; caso contrário: aguardea até que a *mutex* esteja liberada para uso.


Vamos utilizar o mesmo código acima e o modificar para implementar *mutexes*. 


```c
// Logo após as importações da bibliotecas 
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// [...]

// Na função increment modificamos 
    pthread_mutex_lock(&mutex);
    while(i++ < qtd)
        count++;
    pthread_mutex_unlock(&mutex);
    return ((void*)0);
```

Compilamos e executamos. Como se poder, a versão com *mutexes* comportou conforme esperado. 

```shell
❯ ./race 100000
Count: 159154
❯ ./race_mutexes 100000
Count: 500000
```

Explicando: 

`pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER`: 
`pthread_mutex_lock()`: Aqui está adquirindo o *lock* de uma varíavel *mutex*.
`pthread_mutex_unlock(&mutex)` Desbloqueia p *lock*. 



# Referências 

- https://fidelissauro.dev/concorrencia-paralelismo/ 
- http://www.yolinux.com/TUTORIALS/LinuxTutorialPosixThreads.html 
- https://github.com/wdalmorra/Problema-Produtor-Consumidor/tree/master
- https://hpc-tutorials.llnl.gov/posix/ 
- https://wiki.inf.ufpr.br/maziero/lib/exe/fetch.php?media=socm:socm-12.pdf
