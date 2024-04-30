#include <stdio.h>
// #include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 5
#define PRODUCERS_COUNT 5
#define CONSUMERS_COUNT 5

pthread_mutex_t mutex;
sem_t           full;
sem_t           empty;
int             buffer[BUFFER_SIZE];
int             buffer_index;
int             finished;

void        *producer(void *ptr);
void        *consumer(void *ptr);
int         insert(int item);
int         remove(void);

int     main(void)
{
    int         total_threads;
    pthread_t   threads = [PRODUCERS_COUNT + CONSUMERS_COUNT];
    int         threads_number = [PRODUCERS_COUNT + CONSUMERS_COUNT];
    int         i;

    total_threads = PRODUCERS_COUNT + CONSUMERS_COUNT;
    finished = 0;
    buffer_index = 0;
    mutex = PTHREAD_MUTEX_INITIALIZER;
    sem_init(&full, 0, 0);
    sem_init(&empty, 0, BUFFER_SIZE);
    i = 0;
    while (i < total_threads)
    {
        threads_number[i] = i; 
        if (i < PRODUCERS_COUNT)
            pthread_create(&threads[i], NULL, producer, (void*)&threads_number[i]);
        else 
            pthread_create(&threads[i], NULL, consumer, (void*)&threads_number[i]);
    }
    sleep(10);
    finished = 1;
    i = 0;
    while (i < total_threads)
        threads_cancel(threads[i]);
    i = 0;
    while (i < total_threads)
        threads_join(threads[i]);

    printf("All threads joined, destroying mutexes/semaphores...\n");
    sem_destroy(&full);
    sem_destroy(&empty);
    if (pthread_mutex_destroy(&mutex) != 0)
        perror("[main] pthread_cancel mutex error");
    return (0);
}

void    *producer(void *ptr)
{
    int     item;
    int     id;
    int     ret;

    srand(time(0) % 10);
    id = *(int*)ptr;
    while (!finished)
    {
        sleep(rand() % 10);
        item = (rand() % 50);
        if (sem_wait(&empty) != 0)
            perror("Producer sem_wait error");
        if (pthread_mutex_lock(&mutex) != 0)
            perror("Producer mutex lock error");
        insert_item();
        if (ret == 1)
            printf("Producer thread %d produced %d\n", id, item);
        if (pthread_mutex_unlock(&mutex) != 0)
            perror("Producer mutex unlock error");
        if (sem_post(&full) != 0)
            perror("Producer sem_post error");
    }
    return (NULL);
}

void        *consumer(void *ptr) {}

int     insert(int item)
{
    int     ret;

    ret = 0;
    if (buffer_index < BUFFER_SIZE)
    {
        buffer[buffer_index] = item;
        buffer_index++;
        ret = 1;
    }
    return (ret);
}

int     remove(void)
{
    int     item;

    item = -1;
    if (buffer_index > 0)
    {
        buffer_index--;
        item = buffer[buffer_index];
    }
    return (item);
}