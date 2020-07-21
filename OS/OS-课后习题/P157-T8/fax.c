#include "stdio.h"
#include "stdlib.h"
#include "fcntl.h"
#include "unistd.h"
#include "sys/types.h"
#include "pthread.h"
#include "signal.h"
#include "semaphore.h"
#include "time.h"

#define NON_BUSY      1
#define BUSY          0 

/* 信号量 */
sem_t fax_state;


/* 优化 */
//1.有限等待 -> sem_timedwait
struct timespec wait={
    .tv_sec=0,
    .tv_nsec = 1000*1000//1ms
};



/* 线程定义 */
#define PTHREAD_NUM 3    //3 pthread
struct pthread{
    pthread_t p;
    void* (*task)(void*);
};

struct pthread p[PTHREAD_NUM];

/* 理发师问题 */
void p1(void *arg)  
{
    while(1)
    {
        sem_wait(&fax_state);
        printf("thread 1 is using fax!\r\n");
        sem_post(&fax_state);
        printf("thread 1 was left!\r\n");

        sleep(1);
    }
}
void p2(void *arg)  
{
    while(1)
    {
        sem_wait(&fax_state);
        printf("thread 2 is using fax!\r\n");
        sem_post(&fax_state);
        printf("thread 2 was left!\r\n");

        sleep(1);
    }
}
void p3(void *arg)  
{
    while(1)
    {
        sem_wait(&fax_state);
        printf("thread 3 is using fax!\r\n");
        sem_post(&fax_state);
        printf("thread 3 was left!\r\n");
        
        sleep(1);
    }  
}
/*
void p4(void *arg)  
{
    while(1)
    {
        
    }  
}
void p5(void *arg)  
{
    while(1)
    {
        
    }  
}
*/
/* 模拟并发 */
int parbegin(void *arg)
{
    /* 线程结构体初始化 */
    p[0].task = (void*)p1;
    p[1].task = (void*)p2;
    p[2].task = (void*)p3;
    //p[3].task = (void*)p4;
    //p[4].task = (void*)p5;

    for(int i=0;i<PTHREAD_NUM;i++)
    {
        pthread_create(&p[i].p,NULL,p[i].task,arg);
    }
    pthread_exit(NULL);
    return 0;
}

int main(int argc,char** argv)
{
    /* 信号量初始化 */
    sem_init(&fax_state,0,NON_BUSY);

    parbegin(argv[1]);

    return 0;
}