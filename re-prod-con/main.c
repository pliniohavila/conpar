#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pthread.h>
#include <assert.h>
#include <errno.h>

#define MAX_LINES 5
#define MAX_LINE_LENGTH 256

void    *producter_worker(void*);
void    save_line(void);
void    *consumer_work(void*);
char    *consume_line(void);
void    compute_string(char*);

pthread_mutex_t     data_protect        = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t     response_protect    = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t      cond                = PTHREAD_COND_INITIALIZER;
int                 finished            = 0;
int                 buf_filled          = 0;
int                 buf_empty           = 1;
int                 lines_size          = 0;
int                 counter_str         = 0;
char                *lines[MAX_LINES];
FILE                *arq;
char                str_search[80];


int     main(int argc, char **argv)
{
    int             rc;
    int             threads_number;
    pthread_t       producter;
    int             i;
 
    strcpy(str_search, argv[2]);
    if (argc < 2)
        exit(1);
    threads_number = sysconf(_SC_NPROCESSORS_ONLN);
    if (threads_number == 0)
        threads_number = 1;
    pthread_t       consumers[threads_number];
    rc = pthread_create(&producter, NULL, producter_worker, (void*)argv[1]);

    i = 0;
    while (i < threads_number)
    {
        rc = pthread_create(&consumers[i], NULL, consumer_work, NULL);
        assert(rc == 0);
        i++;
    }

    pthread_join(producter, NULL);
    i = 0;
    while (i < threads_number)
    {
        pthread_join(consumers[i], NULL);
        i++;
    }

    printf("%d ocorrências da string [%s]\n", counter_str, str_search);

    return (0);
}

void    *producter_worker(void *filename)
{
    int     fd;

    pthread_mutex_lock(&data_protect);
    arq = fopen((char*)filename, "r");
    fd = fileno(arq);
    if (fd < 0) exit(1);
    pthread_mutex_unlock(&data_protect);

    while(buf_empty) 
    {
        pthread_mutex_lock(&data_protect);
        while (buf_filled)
            pthread_cond_wait(&cond, &data_protect);
        save_line();
        if (lines_size == 5)
        {
            buf_empty = 0;
            pthread_cond_signal(&cond);
            // pthread_mutex_unlock(&data_protect);
            // break;
        }
        pthread_mutex_unlock(&data_protect);
    }
    return (void*)0;
    // pthread_exit(NULL);
}

void    *consumer_work(void *argv)
{
    (void)argv;
    static int  counter_threads = 0;
    char        *s;

    counter_threads++;
    while (buf_filled)
    {
        pthread_mutex_lock(&data_protect);
        while(buf_empty)
            pthread_cond_wait(&cond, &data_protect);
        buf_filled = 0;
        s = consume_line();
        
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&data_protect);
        
        compute_string(s);
    }
    
    printf("Thread: %d\n", counter_threads);
    return (void*)0;
    // pthread_exit(NULL);
}

void    save_line(void)
{
    char        *s;
    size_t      n;

    s = NULL;
    n = 0;
    if (getline(&s, &n, arq) == -1)
    {
        if (feof(arq)) {
            if (s != NULL) free(s);
            return;
        } else {
            perror("getline");
            exit(errno);
        }
    }
    lines[lines_size] = s;
    lines_size++;
    if (lines_size == 5)
        buf_filled = 1;
    return;
}

char    *consume_line(void) 
{
    static int  index = 0;
    char        *s;

    if (!buf_empty)
    {
        s = lines[index];
        index++;
        lines_size--;

        if (lines_size <= 0) 
        {
            buf_empty = 1;
            index = 0;
        }
    }
    return (s);
}

void    compute_string(char *string)
{
    char    *substr;

    if (strlen(string) == 0)
        return;
    substr = strstr(string, str_search);
    printf("[compute_string]substr: %s\n", substr);
    if (substr != NULL)
    {
        pthread_mutex_lock(&response_protect);
        counter_str++;
        pthread_mutex_unlock(&response_protect);
        pthread_mutex_lock(&data_protect);
        if (string != NULL) free(string);
        pthread_mutex_unlock(&data_protect);
    }
}