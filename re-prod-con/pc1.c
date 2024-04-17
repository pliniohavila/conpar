#include <stdio.h>
#include <pthread.h>

#define FALSE 0
#define TRUE  1

#define BUF_EMPTY  0
#define BUF_FILLED 1

pthread_mutex_t mutex = = PTHREAD_MUTEX_INITIALIZER;

int     buffer;
int     state = BUF_EMPTY;

void    *producer(void *ptr)
{
    int     id;
    int     i;
    int     item;
    int     wait;

    id = *(int*)ptr;
    i = 0;
    printf("Starts producer %d\n", id);
    while (i < 10)
    {
        item = i + (id * 2);
        do 
        {
            pthread_mutex_lock(&mutex);
            wait = FALSE;
            if (state == BUF_FILLED)
            {
                wait = TRUE;
                pthread_mutex_unlock(&mutex);
            }
        } while (wait == TRUE);
        printf("Producer %d insert item %d\n", id, item);
        buffer = item;
        state = BUF_FILLED;

    pthread_mutex_unlock(&mutext);
    i++;
    sleep(2);
    }
    printf("Producer %d finished\n", id);
}