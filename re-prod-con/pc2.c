#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>

#define BUFFER_SIZE 3

pthread_mutex_t     mutex;
sem_t               empty;
sem_t               full;
int                 buffer[BUFFER_SIZE];
int                 buffer_index;
bool                interrupt_flag = false;

void        handle_ctrlc(int signal)
{
    interrupt_flag = true;
    printf("ctrl received %d\n", signal);
}

int         insert_item(int item)
{
    if (buffer_index < BUFFER_SIZE)
    {
        buffer[buffer_index] = item;
        buffer_index++;
        return(0);
    }
    return (-1);
}

int         remove_item(void)
{
    int     ret;

    ret = -1;
    if (buffer_index > 0)
    {
        buffer_index--;
        ret = buffer[buffer_index];    
    }
    return (ret);
}

void        *producer(void *producer_number)
{
    int     item;
    int     ret;

    srand(time(0) % 50);
    while(!interrupt_flag)
    {
        sleep(rand() % 10);
        item = rand() % 50;
        if (sem_wait(&empty) != 0)
            perror("Producer sem_wait error");

        if (pthread_mutex_lock(&mutex) != 0)
            perror("Producer pthread_mutex_lock error");
        ret = insert_item(item);

        if (ret == 0)
            printf("Producer thread %d produced %d\n", *(int*)producer_number, item);
        else 
            printf("Full buffer\n");

        if (pthread_mutex_unlock(&mutex) != 0)
            perror("Producer pthread_mutex_unlock error");
        if (sem_post(&full) != 0)
            perror("Producer sem_post error");
    }
    return (NULL);
}

void        *consumer(void *consumer_number)
{
    int     ret;

    srand(time(0) % 10);
    while (!interrupt_flag)
    {
        sleep(rand() % 10);
        if (sem_wait(&full) != 0)
            perror("Consumer sem_wait error");

        if (pthread_mutex_lock(&mutex) != 0)
            perror("Consumer pthread_mutex_lock error");

        ret = remove_item();
        if (ret != 1)
            printf("Consumer thread %d consumed %d\n", *(int *)consumer_number, buffer[buffer_index]);
        else 
            printf("Buffer empty\n");

        if (pthread_mutex_unlock(&mutex) != 0)
            perror("Consumer pthread_mutex_unlock error");
        if (sem_post(&empty) != 0)
            perror("Consumer sem_post error");
    }
    return (NULL);
}

int         main(void)
{
    int     sleep_time;
    int     n_producers;
    int     n_consumers;
    int     total_threads; 

    sleep_time = 10;
    n_producers = 5;
    n_consumers = 5;
    total_threads = n_producers + n_consumers;
    buffer_index = 0;

    pthread_t   threads[total_threads];
    int         threads_number[total_threads];

    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);

    for (int i = 0; i < total_threads; i++)
    {
        threads_number[i] = i;
        if (i < n_producers)
            pthread_create(&threads[i], NULL, producer, (void*)&threads_number[i]);
        else 
            pthread_create(&threads[i], NULL, consumer, (void*)&threads_number[i]);
    }

    if (signal(SIGINT, handle_ctrlc) == SIG_ERR)
        perror("main signal error");
    sleep(sleep_time);
    interrupt_flag = true;
    for (int i = 0; i < total_threads; i++)
        pthread_cancel(threads[i]);
    for (int i = 0; i < total_threads; i++)
        pthread_join(threads[i], NULL);
    printf("All threads joined, destroying mutexes/semaphores...\n");

    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);
    return (0);
}