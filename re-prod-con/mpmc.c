#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
// gcc mpmc.c -fsanitize=thread -pthread -g3 -o mpmc
#define PRODUCER_COUNT  4
#define CONSUMER_COUNT  4
#define THREAD_COUNT    (PRODUCER_COUNT + CONSUMER_COUNT)
#define WORK_ITERS      1000
#define WORK_COUNT      1000

#define QUEUE_SIZE      64

static pthread_mutex_t  queue_head_lock = PTHREAD_MUTEX_INITIALIZER;
static volatile int     queue_head      = 1;

static pthread_mutex_t  queue_tail_lock = PTHREAD_MUTEX_INITIALIZER;
static volatile int     queue_tail      = 1;

static float            queue[QUEUE_SIZE];

static void     *producer(void *data_ptr);
static void     *consumer(void *data_ptr);

int     main(void)
{
    pthread_t       threads[THREAD_COUNT];
    int             data[THREAD_COUNT];
    int             thread_id;

    for (thread_id = 0; thread_id < PRODUCER_COUNT; thread_id++)
    {
        data[thread_id] = thread_id;
        pthread_create(&threads[thread_id], NULL, producer, (void*)&data[thread_id]);
    }

    for (; thread_id < THREAD_COUNT; thread_id++)
    {
        data[thread_id] = thread_id;
        pthread_create(&threads[thread_id], NULL, consumer, (void*)&data[thread_id]);
    }

    for (int i = 0; i < THREAD_COUNT; i++)
        pthread_join(threads[i], NULL);

    return (0);
}

static inline int advance(volatile int *idx)
{
    int     old;
    int     new;
    do 
    {
        old = *idx;
        new = (old + 1) % QUEUE_SIZE;
    } while (!__sync_bool_compare_and_swap(idx, old, new));
    return (old);
}

static inline float do_work(int in)
{
    float w = (float)in;
    for (int i = 0; i < 10; i++)
        w += w;
    return (w);
}

static void *producer(void *data_ptr)
{
    int     thread_id = *(int*)data_ptr;
    float   value;

    printf("[%d] producing\n", thread_id);
    for (int i = 0; i < WORK_COUNT; i++)
    {
        value = do_work(i);

        while (1)
        {
            pthread_mutex_lock(&queue_head_lock);
            if ((queue_head + 1) % QUEUE_SIZE != queue_tail)
                break;
            pthread_mutex_unlock(&queue_head_lock);
            sleep(0);
        }
        queue[queue_head] = value;
        advance(&queue_head);
        pthread_mutex_unlock(&queue_head_lock);
    }
    printf("[%d] finished producing\n", thread_id);
    return (NULL);
}

static void *consumer(void *data_ptr)
{
    int     thread_id = *(int*)data_ptr;
    float   data;
    int     idx;
    float   result;

    printf("[%d] consuming\n", thread_id);
    for (int i = 0; i < WORK_COUNT; i++)
    {
        while (1)
        {
            pthread_mutex_lock(&queue_tail_lock);
            if (queue_tail != queue_head)
                break;
            pthread_mutex_unlock(&queue_tail_lock);
            sleep(1);
        }

        idx = advance(&queue_tail);
        data = queue[idx];
        pthread_mutex_unlock(&queue_tail_lock);
        result += do_work(data);
    }
    printf("[%d] finished consuming result=%.3f\n", thread_id, result);
    return (NULL);
}