# Threads na linguagem C

Repositório destinado aos meus estudos introdutórios de threads na linguagem C. 

## O que são threads?

Vamos aos ensinamentos do mestre Tanenbaum (2016) no seu livro Sistemas Operacionais Modernos que escreve que "_Um processo tem um espaço de endereçamento contendo o código e dados do programa, assim, como outros recursos_". Em seguida, ele nos apresenta o conceito de _threads_: "_um processo tem uma linha de execução (thread) de execução, normalmente abrevidado para apenas **thread**. O thread tem um contador de programa que controla qual instrução deve ser executada em seguida_".

O autor pontua que "_Processos são usados para agrupar recursos; threads são as entidades escalonadas para execução na CPU_". Por fim, complementando, "_os threads compartilham um espaço de endereçamento e outros recursos_" e "_processos compartilham memórias físicas, discos, impressoras e outros recursos_". 

Para este estudo estarei utilizando a Linguagem C e a biblioteca `POSIX threads`. Que segundo a sua documentação (`man pthreads`): 
>   POSIX.1  specifies a set of interfaces (functions, header files) for threaded programming commonly known as POSIX threads, or Pthreads.  A single
    process can contain multiple threads, all of which are executing the same program.  These threads share the same global  memory  (data  and  heap
    segments), but each thread has its own stack (automatic variables).

Para utilizar, adicionamos a flag `-pthread` ao compilar os nossos códigos. 


[COLOCAR UM CÓDIGO SIMPLES DE UMA THREAD]



[EXPLICAÇÃO SOBRE AS FUNÇÕES UTILIZADAS]

|Função|Descrição|
| --- | --- |
|`pthread_create`|Cria uma nova thread|
|`pthread_exit`|Conclui uma chamada de thread|
|`pthread_join`|Espera que um thread específico seja abandonado|


# Sincronização de threads 

## Race Conditions

Como vimos acima sobre `threads`, que diferente de processos, `threads` compartilham o mesmos recursos disponíveis para o processo a qual pertencem. Nesse cenário, pode acontecer comportamentos inesperados quando `threads` acessam um mesmo recurso ao mesmo tempo ou mesma região da memória. 

A esses casos são denominados *race conditions*. 

Podemos dizer que está ocorrendo uma *race condition* quando "_dois ou mais processos estão lendo ou escrevendo alguns dados compartilhados e o resultado final depende de quem executa precisamente e quando_", Tanenbaum (2016).

Coleciono também, o conceito presente em [POSIX thread (pthread) libraries](http://www.yolinux.com/TUTORIALS/LinuxTutorialPosixThreads.html).

>A contention or race condition often occurs when two or more threads need to perform operations on the same memory area, but the results of computations depends on the order in which these operations are performed. Mutexes are used for serializing shared resources such as memory. 

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

Aproveitar e colocar uma nota sobre o modificador `volatile`. 

Ele indica para o compilador que variável pode ser modificada por fatores externos, aplicando uma restrição às otimições realizadas pelo compilador. 


### ThreadSanitizer

Uma ferramenta que pode nos ajudar é ThreadSanitizer.


Para utilizarmos, compilarmos nosso código com flag `-fsanitize=thread`. 
Ao executar, é disparado um aviso que está ocorrendo uma condição de corrida à variável `count`. Vejamos: 

```shell 
❯ ./race
==================
WARNING: ThreadSanitizer: data race (pid=14464)
  Read of size 4 at 0x56453e338014 by thread T2:
    #0 increment <null> (race+0x141a)

  Previous write of size 4 at 0x56453e338014 by thread T1:
    #0 increment <null> (race+0x1432)

  Location is global 'count' of size 4 at 0x56453e338014 (race+0x000000004014)

  Thread T2 (tid=14467, running) created by main thread at:
    #0 pthread_create ../../../../src/libsanitizer/tsan/tsan_interceptors_posix.cpp:969 (libtsan.so.0+0x605b8)
    #1 main <null> (race+0x1333)

  Thread T1 (tid=14466, finished) created by main thread at:
    #0 pthread_create ../../../../src/libsanitizer/tsan/tsan_interceptors_posix.cpp:969 (libtsan.so.0+0x605b8)
    #1 main <null> (race+0x1333)

SUMMARY: ThreadSanitizer: data race (/mnt/c/Users/win11/lab/conpar/yolinux/race+0x141a) in increment
==================
Count: 27076
ThreadSanitizer: reported 1 warnings
```

Por mais informações, veja: <https://www.cs.columbia.edu/~junfeng/11fa-e6121/papers/thread-sanitizer.pdf>

Diante desse cenários que temos a **sincronização de *threads***. 

## Mecanismos para lidar com sincronização de threads

A biblioteca `POSIX threads` fornece três mecanismo para lidar com sincronização de threads. São: 

| Mecanismo | Descrição |
| --- | --- |
| Mutexes (Mutual exclusion lock) | Bloqueia o acesso a determinada variável/recurso para outros threads |
| Joins | Faz com que um `thread` aguarde que outras estejam terminadas |
| Variáveis de condição | Utiliza o tipo `pthread_cond_t` para controlar o acesso a determinada variável/recurso |

### Mutexes 

*Mutexes* são utilizados para evitar comportamentos inesperados ou inconsistências com os dados durante operações de vários `threads`na mesma área de memória ou recurso. É uma forma de prevenir *race conditions*. 

O seu uso se dá por meio de uma variável do tipo `pthread_mutex_t`. Quando um `thread` tentar acessar uma área de memória, primeiro ela tentar bloquear a variável *mutex*; caso consiga: realiza sua ação; caso contrário: aguarda até que a *mutex* esteja liberada para uso.

Mais um vez, trago aqui os ensinamentos do mestre Tanenbaum (2016):

> Um **mutex** é uma variável compartilhada que pode estar em um de dois estados: destravado ou travado. Em consequência, apenas 1 bit é necessário para representá-lo, mas na prática muitas vezes um inteiro é usado, com 0 significando destravado e todos os outros valores significando travado. 

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

|Função / tipo| Descrição |
| --- | --- |
|`PTHREAD_MUTEX_INITIALIZER` | Macro utilizado para inicializar uma _mutex_ | 
|`pthread_mutex_lock()` | Aqui está adquirindo o *lock* de uma variável *mutex* |
|`pthread_mutex_unlock(&mutex)` | Desbloqueia o *lock* |

### Join

Utilizamos o mecanismo _join_ quando desejamos aguardar que um thread seja finalizado antes de continuar com a execução do código. É semelhante às chamadas das funções `wait` e `waitpid` no contexto de processos. 

Para isso fazemos uso da função `pthread_join`, que atua quando "_um thread precisa esperar outro terminar seu trabalho e sair antes de continuar. O que esperando chamada `pthread_join` para esperar outro thread específico terminar_" Tanenbaum (2016);

Indicamos qual thread é para aguardar passando como primeiro argumento da função. O segundo argumento da função é informado quando queremos recuperar algum valor retornado pelo thread. Esse argumento é facultativo, caso não se tenha valor para recuperar, passamos `NULL` como argumento. 

É assinatura da função 

```c 
int pthread_join(pthread_t thread, void **retval);
```

Vejamos um exemplo o uso de _join_

```c 
#include <stdio.h>
#include <pthread.h>

#define NTHREADS 10
void                *thread_function(void*);
pthread_mutex_t     mutex = PTHREAD_MUTEX_INITIALIZER;
int                 counter = 0;

int     main(void)
{
    pthread_t   thread_id[NTHREADS];
    int         i;

    i = 0;
    while (i < NTHREADS)
    {
        pthread_create(&thread_id[i], NULL, thread_function, NULL);
        i++;
    }
    i = 0;
    while (i < NTHREADS)
    {
        pthread_join(thread_id[i], NULL);
        i++;
    }
    printf("Counter: %d\n", counter);
    return (0);
}

void    *thread_function(void*)
{
    printf("Thread number: %ld\n", pthread_self());
    pthread_mutex_lock(&mutex);
    counter++;
    pthread_mutex_unlock(&mutex);
    return ((void*)0);
}
```
No exemplo acima, inicializamos 10 threads, que chamam a função `thread_function`, que por sua vez, incrementa o valor de `counter`, e depois aguardamos o encerramento de cada uma dos threads inicializados para depois continuar com o fluxo de execução do código. 

Observa que aqui também utilizamos _mutexes_ para controlar as alterações da variável `counter`, e assim evitar que o programa se comporte de forma inesperada: seja deixando de fazer uma incrementação, seja incrementando a mais. Bem, que nesse caso, como a quantidade de incrementações é pífia a chance de ocorrer uma condição de corrida é baixa. 

Uma explicação à chamada da função `pthread_self()`: ela retorna o ID da thread chamada, é mesmo valor que retornado por uma chamada `pthread_create(3)` quando criamos um thread. 

### Variáveis de Condição 

Variáveis de condição são variáveis que suspende a execução do thread até que determinada condição seja atendida. Segundo Tanenbaum (2016), "_variáveis de condição permitem que threads sejam bloqueados devido a alguma condição não estar sendo atendida_".

Deve-se atentar, que quando estamos lidando com variável de condição, sempre devemos associar ela com um mutex, o qual será responsável por bloquear a região crítica. 

Acrescentando, segundo o OSTEP: a "_condition variable is an explicit queue that threads can put themselves on when some state of execution (i.e., some condition) is not as desired (by waiting on the condition); some other thread, when it changes said state, can then wake one (or more) of those waiting threads and thus allow them to continue (by signaling on the condition)_".

Abaixo temos o código que utiliza variável de condição e mutex. Fazemos uso de dois threads. A execução varia entre a execução de cada thread, cada execução realiza uma atividade de incremento da variável `count`. Utilizamos a variável de condição para controlar a execução dos threads. 

```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t count_mutex         = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  condition_var       = PTHREAD_COND_INITIALIZER;

#define     COUNT_DONE      10
#define     COUNT_HALT1     3
#define     COUNT_HALT2     6

void        *fcount1(void*);
void        *fcount2(void*);

int     main(void)
{
    volatile int    count;
    pthread_t       th1;
    pthread_t       th2;

    count = 0;
    pthread_create(&th1, NULL, fcount1, (void*)&count);
    pthread_create(&th2, NULL, fcount2, (void*)&count);

    pthread_join(th1, NULL);
    pthread_join(th2, NULL);

    printf("Final count: %d\n", count);
    return (0);
}

void        *fcount1(void *ptr) 
{
    int     *to_increment;

    to_increment = (int*)ptr;
    while (1)
    {
        pthread_mutex_lock(&count_mutex);
        pthread_cond_wait(&condition_var, &count_mutex);
        *to_increment += 1;
        printf("[fcount1] value: %d\n", *to_increment);
        pthread_mutex_unlock(&count_mutex);
        if (*to_increment >= COUNT_DONE)
            pthread_exit(NULL);
    }
}

void        *fcount2(void *ptr) 
{
    int     *to_increment;

    to_increment = (int*)ptr;
    while (1)
    {
        pthread_mutex_lock(&count_mutex);
        if (*to_increment < COUNT_HALT1 || *to_increment > COUNT_HALT2)
            pthread_cond_signal(&condition_var);
        else 
        {
            *to_increment += 1;
            printf("[fcount2] value: %d\n", *to_increment);
        }
        pthread_mutex_unlock(&count_mutex);
        if (*to_increment >= COUNT_DONE)
            pthread_exit(NULL);
    }
} 
```

Explicando as funções acima uti

O macro `PTHREAD_COND_INITIALIZER`: 

A função `pthread_cond_wait`: 
http://man.yolinux.com/cgi-bin/man2html?cgi_command=pthread_cond_wait

A função `pthread_cond_signal` 
http://man.yolinux.com/cgi-bin/man2html?cgi_command=pthread_cond_signal

Uma observação quanto ao uso de `while` e porque não podemos utilizar `if`. 

## Semáforos 

Outro mecanismo para controle de threads, são os semáforos. Bem o que são? 

Desenvolvidos pelo cara, o E. W. Dijkstra, é a utilização de uma variável inteira para contar o número de sinais de acordar salvos para uso futuro, assim ensina o Tanenbaum (2016).

Realizamos duas operações básicas sobre semáforos: incrementar ou decrementar. 
Quando desejamos sinalizar que operações estão disponíveis para serem realizar, incrementamos o valor do semáforo. 
E a cada vez que um operação é realizada, o decrementamos até chegar o valor de zero. 
O ato de decrementar o semáforo dever ser realizado antes de realizar alguma atividade. Funciona como uma verificação de disponibilidade. 

Se após, o valor for superior a (-1), o thread segue o fluxo de execução, caso contrário, ela vai dormir até ser acordado novamente para uma nova tentativa. 

# Referências 

- Tanenbaum, Andrew S. Sistemas operacionais modernos / Andrew S. Tanenbaum, Herbert Bos; tradução Jorge Ritter; revisão técnica Raphael Y. de Camargo. –  4. ed. – São Paulo: Pearson Education do Brasil, 2016
- https://fidelissauro.dev/concorrencia-paralelismo/ 
- http://www.yolinux.com/TUTORIALS/LinuxTutorialPosixThreads.html 
- https://github.com/wdalmorra/Problema-Produtor-Consumidor/tree/master
- https://hpc-tutorials.llnl.gov/posix/ 
- https://wiki.inf.ufpr.br/maziero/lib/exe/fetch.php?media=socm:socm-12.pdf
- https://pages.cs.wisc.edu/~remzi/OSTEP/threads-cv.pdf
- https://www.cs.columbia.edu/~junfeng/11fa-e6121/papers/thread-sanitizer.pdf 
- https://pt.stackoverflow.com/questions/10175/para-que-serve-o-modificador-volatile-do-c-c
- https://embarcados.com.br/modificadores-de-acesso-na-linguagem-c/