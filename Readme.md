# Concorrência e Paralelismo 

Repositório destinado aos meus estudos introdutórios de concorrência e paralelismo na linguagem C. 



# Sincronização de threads 

[falar sobre o problema de *race conditions*]

Exemplificar com código de alteração do valor de uma variável por várias `threads`.

O código abaixo, temos uma variável global `count` que será incrementada `n` vezes por 5 `threads` "ao mesmo tempo". 

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


Aproveitar e colocar uma nota sobre a palavra chave `volatile`. 


A biblioteca para lidar com `threads` fornece três mecanismo para lidar com sincronização de `threads`. São: 
1. mutexes(Mutual exclusion lock)
2. joins 
3. condition variables   


# Referências 

- https://fidelissauro.dev/concorrencia-paralelismo/ 
- http://www.yolinux.com/TUTORIALS/LinuxTutorialPosixThreads.html 
- https://github.com/wdalmorra/Problema-Produtor-Consumidor/tree/master
- https://hpc-tutorials.llnl.gov/posix/ 
- https://wiki.inf.ufpr.br/maziero/lib/exe/fetch.php?media=socm:socm-12.pdf
