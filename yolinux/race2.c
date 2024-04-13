#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

// volatile unsigned int count = 0;
void    *increment(void*);

int     main(void)
{
    pthread_t       threads[5];
    int             i;
    volatile int    count = 0;

    i = 0;
    count = 0;
    while (i < 5) 
    {
        pthread_create(&threads[i], NULL, increment, (void*)&count);
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

    i = 0;
    while(i++ < 10)
        (*(int*)ptr)++;
    return ((void*)0);
}
