
#define BUFFER_SIZE 5

pthread_mutex_t     mutex   = PTHREAD_MUTEX_INITIALIZER;


void        *producer(void *ptr);
void        *consumer(void *ptr);
int         insert(int item);
int         remove(void);


int     main(void)
{


    if (pthread_cancel(&mutex) != 0)
        perror("[main]pthread_cancel mutex error");
    return (0);
}

