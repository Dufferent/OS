#include "stdio.h"
#include "stdlib.h"
#include "fcntl.h"
#include "unistd.h"
#include "sys/types.h"
#include "pthread.h"
#include "signal.h"
#include "semaphore.h"
#include "time.h"

#define NON_BUSY 1
#define BUSY     0 


/* 共享信息 */
int share = 1;

/* 信号量 */
#define Reader 3
#define Writer 2

sem_t w_lock;
sem_t r_lock[3];

/* 优化 */
//1.有限等待 -> sem_timedwait
struct timespec wait={
    .tv_sec=0,
    .tv_nsec = 1000*1000//1ms
};
/* 线程定义 */
#define PTHREAD_NUM 5    //5 pthread
struct pthread{
    pthread_t p;
    void* (*task)(void*);
};

struct pthread p[PTHREAD_NUM];

/* 读写进程 */
void p1(void *arg)  //write
{
    while(1)
    {
        
        for(int i=0;i<Reader;i++)
            sem_wait(&r_lock[i]);
        sem_wait(&w_lock);
        /* 临界区 */
        share = (share+1)%11;
        sem_post(&w_lock);
        for(int i=0;i<Reader;i++)
            sem_post(&r_lock[i]);
            
        usleep(500*1000);
    }
}
void p2(void *arg)  //write
{
    while(1)
    {
        
        for(int i=0;i<Reader;i++)
            sem_wait(&r_lock[i]);
        sem_wait(&w_lock);
        /* 临界区 */
        share = (share+1)%11;
        sem_post(&w_lock);
        for(int i=0;i<Reader;i++)
            sem_post(&r_lock[i]);

        usleep(500*1000);
    }
}
void p3(void *arg)  //read
{
    while(1)
    {
        sem_wait(&r_lock[0]);
        printf("share data:%d\r\n",share);
        sem_post(&r_lock[0]);
        usleep(500*1000);
    }  
}
void p4(void *arg)  //read
{
    while(1)
    {
        sem_wait(&r_lock[1]);
        printf("share data:%d\r\n",share);
        sem_post(&r_lock[1]);
        usleep(500*1000);
    }  
}
void p5(void *arg)  //read
{
    while(1)
    {
        sem_wait(&r_lock[2]);
        printf("share data:%d\r\n",share);
        sem_post(&r_lock[2]);
        usleep(500*1000);
    }  
}

/* 模拟并发 */
int parbegin(void *arg)
{
    /* 线程结构体初始化 */
    p[0].task = (void*)p1;
    p[1].task = (void*)p2;
    p[2].task = (void*)p3;
    p[3].task = (void*)p4;
    p[4].task = (void*)p5;

    for(int i=0;i<PTHREAD_NUM;i++)
    {
        pthread_create(&p[i].p,NULL,p[i].task,arg);//哲学家
    }
    pthread_exit(NULL);
    return 0;
}

int main(int argc,char** argv)
{
    /* 信号量初始化 */
    for(int i=0;i<Reader;i++)
        sem_init(&r_lock[i],0,NON_BUSY);

    sem_init(&w_lock,0,NON_BUSY);

    parbegin(argv[1]);

    return 0;
}
