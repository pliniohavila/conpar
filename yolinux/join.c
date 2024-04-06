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