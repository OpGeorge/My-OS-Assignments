#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "a2_helper.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <pthread.h>

sem_t sem5Inceput;
sem_t sem3Sfarsit;
sem_t *sem2_1Sfrasit;
sem_t *sem6_3Sfrasit;
sem_t sem4Sync;
sem_t sem11Sync;
sem_t semInc;
int nrThreadsin4 = 0;
// if(fork()==0)
//  {
//      create new thread;
//      exit;
//  }
void *thread1Func(void *arg)
{
    sem_wait(sem6_3Sfrasit);

    info(BEGIN, 2, 1);

    info(END, 2, 1);
    sem_post(sem2_1Sfrasit);
    return NULL;
}

void *thread2Func(void *arg)
{
    info(BEGIN, 2, 2);
    info(END, 2, 2);
    return NULL;
}

void *thread3Func(void *arg)
{
    sem_wait(&sem5Inceput);
    info(BEGIN, 2, 3);
    info(END, 2, 3);
    sem_post(&sem3Sfarsit);
    return NULL;
}

void *thread4Func(void *arg)
{
    info(BEGIN, 2, 4);
    info(END, 2, 4);
    return NULL;
}

void *thread5Func(void *arg)
{

    info(BEGIN, 2, 5);
    sem_post(&sem5Inceput);
    sem_wait(&sem3Sfarsit);
    info(END, 2, 5);
    return NULL;
}

void *thread6Func(void *arg)
{
    int i;
    i = *(int *)arg;
    if (i == 5)
    {
        sem_wait(sem2_1Sfrasit);
    }

    info(BEGIN, 6, i);

    info(END, 6, i);

    if (i == 3)
    {
        sem_post(sem6_3Sfrasit);
    }

    return NULL;
}

void *threadP4Func(void *arg)
{
    int i;
    i = *(int *)arg;

    sem_wait(&sem4Sync);

    sem_wait(&semInc);
    nrThreadsin4++;
    sem_post(&semInc);

    if (i == 11)
    {
        sem_wait(&sem11Sync);
    }

    while(i==11 && nrThreadsin4 <4)
        ;
    info(BEGIN, 4, i);

    if (i != 11)
        sem_wait(&sem11Sync);

    info(END, 4, i);

    nrThreadsin4--;

    sem_post(&sem11Sync);

    sem_post(&sem4Sync);
    return NULL;
}

void p8create()
{
    info(BEGIN, 8, 0);
    info(END, 8, 0);
}
void p6create()
{
    info(BEGIN, 6, 0);

    int threadsID[6];
    pthread_t p6Arr[6];

    int i;
    for (i = 0; i < 6; i++)
    {
        threadsID[i] = i + 1;
        pthread_create(&p6Arr[i], NULL, thread6Func, &threadsID[i]);
    }

    for (i = 0; i < 6; i++)
    {
        pthread_join(p6Arr[i], NULL);
    }

    while ((wait(NULL) > 0))
        ;

    info(END, 6, 0);
}

void p3Create()
{
    info(BEGIN, 3, 0);
    info(END, 3, 0);
}

void p5create()
{
    info(BEGIN, 5, 0);

    if (fork() == 0)
    {
        p6create();
        return;
    }

    if (fork() == 0)
    {
        p8create();
        return;
    }
    while ((wait(NULL) > 0))
        ;

    info(END, 5, 0);
}

void p7create()
{
    info(BEGIN, 7, 0);
    info(END, 7, 0);
}

void p2Create()
{
    info(BEGIN, 2, 0);

    pthread_t thread1;
    pthread_t thread2;
    pthread_t thread3;
    pthread_t thread4;
    pthread_t thread5;
    sem_init(&sem3Sfarsit, 0, 0);
    sem_init(&sem5Inceput, 0, 0);

    pthread_create(&thread1, NULL, &thread1Func, NULL);
    pthread_create(&thread2, NULL, &thread2Func, NULL);

    pthread_create(&thread3, NULL, &thread3Func, NULL);
    pthread_create(&thread4, NULL, &thread4Func, NULL);
    pthread_create(&thread5, NULL, &thread5Func, NULL);
    sem_destroy(&sem3Sfarsit);
    sem_destroy(&sem5Inceput);

    if (fork() == 0)
    {
        p3Create();
        return;
    }

    if (fork() == 0)
    {
        p5create();
        return;
    }
    if (fork() == 0)
    {
        p7create();
        return;
    }

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);
    pthread_join(thread4, NULL);
    pthread_join(thread5, NULL);

    while ((wait(NULL) > 0))
        ;

    info(END, 2, 0);
}

void p4create()
{
    info(BEGIN, 4, 0);

    int threadsID[45];
    pthread_t p4Arr[45];
    nrThreadsin4 = 0;
    sem_init(&sem4Sync, 0, 4);
    sem_init(&sem11Sync, 0, 1);
    sem_init(&semInc, 0, 1);
    int i;
    for (i = 0; i < 45; i++)
    {
        threadsID[i] = i + 1;
        pthread_create(&p4Arr[i], NULL, threadP4Func, &threadsID[i]);
    }

    for (i = 0; i < 45; i++)
    {
        pthread_join(p4Arr[i], NULL);
    }

    while ((wait(NULL) > 0))
        ;
    sem_destroy(&sem4Sync);
    sem_destroy(&sem11Sync);
    sem_destroy(&semInc);

    info(END, 4, 0);
}

int main()
{
    init();

    sem_unlink("sem2_1Sfrasit");

    sem_unlink("sem6_3Sfrasit");

    info(BEGIN, 1, 0);
    sem2_1Sfrasit = sem_open("sem2_1Sfrasit", O_CREAT, 0644, 0);

    sem6_3Sfrasit = sem_open("sem6_3Sfrasit", O_CREAT, 0644, 0);

    if (fork() == 0)
    {
        p2Create();
        return 0;
    }
    if (fork() == 0)
    {
        p4create();
        return 0;
    }
    while ((wait(NULL) > 0))
        ;
    info(END, 1, 0);
    sem_close(sem2_1Sfrasit);
    sem_close(sem6_3Sfrasit);
    return 0;
}
