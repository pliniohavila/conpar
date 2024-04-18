//  gcc pc1.c -fsanitize=thread -pthread -g3 -o pc1
#include <stdio.h>
#include <unistd.h>
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
        
        pthread_mutex_lock(&mutex_finish);
        if (producers_finisheds > 2)
            finish = TRUE;
        pthread_mutex_unlock(&mutex_finish);
        // while (!__sync_bool_compare_and_swap(&finish, FALSE, TRUE)) {}
    } 
    printf("Consumer %d finished\n", id);
    return ((void*)0);
}

int     main(void)
{
    pthread_t   producers[3];
    pthread_t   consumers[2];
    int         ths[3] = {1, 2, 3};

    printf("Starts producers and consumrers\n");

    for (int i = 0; i < 3; i++)
        pthread_create(&producers[i], NULL, producer, (void*)&ths[i]);

    for (int i = 0; i < 2; i++)
        pthread_create(&consumers[i], NULL, consumer, (void*)&ths[i]);

    for (int i = 0; i < 3; i++)
        pthread_join(producers[i], NULL);

    for (int i = 0; i < 2; i++)
        pthread_join(consumers[i], NULL);

    return (0);
}