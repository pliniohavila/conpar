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