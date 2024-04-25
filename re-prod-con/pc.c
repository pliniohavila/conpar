/*
    author: Erik Saffor
    gcc pc.c -pthread (threads) -lpthread -lrt (semaphores)

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>

#define         BUFFER_SIZE 5

pthread_mutex_t mutex;
sem_t           empty;
sem_t           full;

typedef int     buffer_item;
buffer_item     buffer[BUFFER_SIZE];
int             buffer_index; 
bool            interrupt_flag = false;

void            handle_ctrlc(int signal)
{
    interrupt_flag = true;
    printf("Ctrl-c received %d, attempting to exit...\n", signal);
}

int             insert_item(buffer_item item)
{
    if (buffer_index < BUFFER_SIZE)
    {
        buffer[buffer_index] = item;
        buffer_index++;
        return (0);
    }
    return (-1);
}

int             remove_item(void)
{
    buffer_item    item;

    if (buffer_index > 0)
    {
        buffer_index--;
        item = buffer[buffer_index];
        return ((int)item);
    }
    return (-1);
}

void            *producer(void *producer_number)
{
    int         ret;
    buffer_item item;

    srand(time(0) % 10);

    while (!interrupt_flag)
    {
        sleep(rand() % 10);

        item = rand() % 50;
        if (sem_wait(&empty) != 0)
            perror("Producer sem_wait erro");
        if (pthread_mutex_lock(&mutex) != 0)
            perror("Producer pthread_mutex_lock error");
        ret = insert_item(item);
        if (ret == 0)
            printf("Producer thread %d produced %d\n", *(int*)producer_number, item);
        else if (ret == -1)
            printf("Full buffer\n");

        if (pthread_mutex_unlock(&mutex) != 0)
            perror("Producer pthread_mutex_unlock error");

        if (sem_post(&full) != 0)
            perror("Producer sem_post error");
    }
    return (NULL);
}

void            *consumer(void *consumer_number)
{
    int     ret;

    srand(time(0) % 10);
    while (!interrupt_flag)
    {
        sleep(rand() % 10);
        // Tenta decrementar o semafóro, se sim: segue o fluxo de execução
        // se não: thread vai dormir 
        if (sem_wait(&full) != 0)
            perror("Consumer sem_wait error");
        if (pthread_mutex_lock(&mutex) != 0)
            perror("Consumer pthread_mutex_lock error");

        ret = remove_item();
        if (ret != -1)
            printf("Consumer thread %d consumed %d\n", *(int *)consumer_number, ret);
        // else if (ret == -1)
        else
            printf("Empty buffer\n");
        if (pthread_mutex_unlock(&mutex))
            perror("Consumer pthread_mutex_unlock error");
        if (sem_post(&empty) != 0)
            perror("Consumer sem_post error");
    }
    return (NULL);
}

int             main(void)
{
    // char    *ptr;
    int     sleep_time;
    int     num_producers;
    int     num_consumers;

    sleep_time = 10;
    num_producers = 5;
    num_consumers = 5;

    printf("Waiting for %d seconds before exiting, with %d producers and %d consumers...\n", sleep_time, num_producers, num_consumers);

    buffer_index = 0;
    pthread_t   threads[num_producers + num_consumers];
    int         thread_number[num_producers + num_consumers];

    if (pthread_mutex_init(&mutex, NULL) != 0)
        perror("main pthread_mutex_init error");

    if (sem_init(&empty, 0, BUFFER_SIZE) != 0)
        perror("main sem_init error (empty)");
    if (sem_init(&full, 0, 0) != 0)
        perror("main sem_init error (full)");
    
    for (int i = 0; i < num_producers + num_consumers; i++)
    {
        thread_number[i] = i;
        if (i < num_producers) 
        {
            if (pthread_create(&threads[i], NULL, producer, (void*)&thread_number[i]) != 0)
                perror("main pthread_create error (producer)");
        }
        else 
        {
            if (pthread_create(&threads[i], NULL, consumer, (void*)&thread_number[i]) != 0)
                perror("main pthread_create error (consumer)");
        }
    }

    if (signal(SIGINT, handle_ctrlc) == SIG_ERR)
        perror("main signal error");
    sleep(sleep_time);
    interrupt_flag = true;
    for (int i = 0; i < num_producers + num_consumers; i++)
    {
        if (pthread_cancel(threads[i]) != 0)
            perror("main pthread_cancel error");
    }
    for (int i = 0; i < num_producers + num_consumers; i++)
    {
        if (pthread_join(threads[i], NULL) != 0)
            perror("main pthread_join error");
    }
    printf("All threads joined, destroying mutexes/semaphores...\n");
    if (sem_destroy(&empty) != 0)
        perror("main sem_destroy error (empty)");
    if (sem_destroy(&full) != 0)
        perror("main sem_destroy error (full)");
    if (pthread_mutex_destroy(&mutex) != 0)
        perror("main mutext destroy error");
    return (0);
}