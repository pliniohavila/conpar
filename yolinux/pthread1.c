#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>


void    *print_function(void*);

int     main(void)
{
    pthread_t       th1;
    pthread_t       th2;
    int             thr1;
    int             thr2;
    const char      *msg1;
    const char      *msg2;

    msg1 = "Thread One";
    msg2 = "Thread Two";
    // TODO: create independent threads each of which will execute function `print_msg`
    thr1 =  pthread_create(&th1, NULL, print_function, (void*)msg1);
    assert(thr1 == 0);
    thr2 =  pthread_create(&th2, NULL, print_function, (void*)msg2);
    assert(thr2 == 0);
    // Show return 
    printf("thr1: %d\n", thr1);
    printf("thr2: %d\n", thr2);

    pthread_join(th1, NULL);
    pthread_join(th1, NULL);
    return (0);
}

// TODO implement `print_msg` function 
void    *print_function(void *ptr)
{
    char    *msg;

    msg = (char*)ptr;
    printf("%s\n", msg);
    return ((void*)0);
}