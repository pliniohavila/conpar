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
