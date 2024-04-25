//  gcc pc1.c -fsanitize=thread -pthread -g3 -o pc1
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define FALSE 0
#define TRUE  1

#define BUF_EMPTY  0
#define BUF_FILLED 1

pthread_mutex_t mutex        = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_finish = PTHREAD_MUTEX_INITIALIZER;

int     buffer;
int     state = BUF_EMPTY;
int     producers_finisheds = 0;
int     finish = FALSE;

void    *producer(void *ptr)
{
    int     id;
    int     i;
    int     item;
    int     wait;

    id = *(int*)ptr;
    i = 0;
    printf("Starts producer %d\n", id);
    while (i < 5)
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
        } while (wait);
        printf("Producer %d insert item %d\n", id, item);
        buffer = item;
        state = BUF_FILLED;

        pthread_mutex_unlock(&mutex);
        i++;
        sleep(1);
    }
    pthread_mutex_lock(&mutex_finish);
    producers_finisheds++;
    pthread_mutex_unlock(&mutex_finish);
    printf("Producer %d finished\n", id);
    return ((void*)0);
}

void    *consumer(void *ptr)
{
    int     id;
    int     item;
    int     wait;

    id = *(int*)ptr;
    printf("Consumer %id starts", id);
    while (!finish)
    {
        printf("Consumer %d finish %d\n", id, finish);
        do
        {
            pthread_mutex_lock(&mutex);
            wait = FALSE;
            if (state == BUF_EMPTY)
            {
                wait = TRUE;
                pthread_mutex_unlock(&mutex);
            }
        } while (wait);
        item = buffer;
        state = BUF_EMPTY;
        pthread_mutex_unlock(&mutex);
        printf("Consumer %d consume item %d\n", id, item);
        sleep(1);
    } 
    printf("Consumer %d finished\n", id);
    return (NULL);
}

int     main(void)
{
    int         ths_numbers;
    pthread_t   threads[6];
    int         ths[6];
    int         i;


    printf("Starts producers and consumrers\n");
    ths_numbers = 6;
    for (i = 0; i < 3; i++)
    {
        ths[i] = i;
        pthread_create(&threads[i], NULL, producer, (void*)&ths[i]);
    }

    for (; i < ths_numbers; i++)
    {
        ths[i] = i;
        pthread_create(&threads[i], NULL, consumer, (void*)&ths[i]);
    }

    sleep(10);
    finish = TRUE;

    for (i = 0; i < ths_numbers; i++)
    {
        if (pthread_cancel(threads[i]) != 0)
        {
            printf("thread[%d] ", i);
            perror("main pthread_cancel error");
        }
    }

    for (i = 0; i < ths_numbers; i++)
    {
        if (pthread_join(threads[i], NULL) != 0)
            perror("main pthread_join error");
    }
    printf("All threads joined, destroying mutexes/semaphores...\n");
    return (0);
}