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

/* buffer */
//（1）
int buffer_AB[1]={0};
int buffer_BC[1]={0};
//（2）
//同生产者消费者问题


/* 信号量 */
sem_t buf_stat_AB;
sem_t buf_full_AB;
sem_t buf_idle_AB;

sem_t buf_stat_BC;
sem_t buf_full_BC;
sem_t buf_idle_BC;
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
void p1(void *arg)      //A
{
    while(1)
    {
        sem_wait(&buf_idle_AB);//空闲的数据缓冲
        sem_wait(&buf_stat_AB);//有人在读写吗
        buffer_AB[0] += 1;     //给数据让B取走
        sem_post(&buf_full_AB);
        sem_post(&buf_stat_AB);

        sleep(1);
    }
}
void p2(void *arg)      //B
{
    int transmit;
    while(1)
    {
        //1.取走AB缓冲里的数据
        sem_wait(&buf_full_AB);//有数据没
        sem_wait(&buf_stat_AB);//有数据我就读啊
        transmit = buffer_AB[0];
        sem_post(&buf_idle_AB);
        sem_post(&buf_stat_AB);

        //2.给C数据
        sem_wait(&buf_idle_BC);//空闲的数据缓冲
        sem_wait(&buf_stat_BC);//有人在读写吗
        buffer_BC[0] = transmit;//给数据让C取走
        sem_post(&buf_stat_BC);
        sem_post(&buf_full_BC);

        sleep(1);
    }
}
void p3(void *arg)      //C
{
    while(1)
    {
        sem_wait(&buf_full_BC);//有数据没
        sem_wait(&buf_stat_BC);//有数据我就读啊
        printf("C rec:%d\r\n",buffer_BC[0]);
        sem_post(&buf_idle_BC);
        sem_post(&buf_stat_BC);

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
    sem_init(&buf_stat_AB,0,1);
    sem_init(&buf_stat_BC,0,1);
    sem_init(&buf_full_AB,0,0);
    sem_init(&buf_full_BC,0,0);
    sem_init(&buf_idle_AB,0,1);
    sem_init(&buf_idle_BC,0,1);

    parbegin(argv[1]);

    return 0;
}