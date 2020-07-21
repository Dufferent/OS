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
#define BUF_TYPE int
BUF_TYPE buffer[500]={0};
static int index = 0;
static int rindex = 0;
/* 信号量 */
sem_t mutex;//缓冲区的读写权限
sem_t buf_full;
sem_t buf_idle;
sem_t r_flag;




/* 优化 */
//1.有限等待 -> sem_timedwait
struct timespec wait={
    .tv_sec=0,
    .tv_nsec = 1000*1000//1ms
};



/* 线程定义 */
#define PTHREAD_NUM 5    //3 pthread
struct pthread{
    pthread_t p;
    void* (*task)(void*);
};

struct pthread p[PTHREAD_NUM];

/* 理发师问题 */
void p1(void *arg)      //pro
{
    while(1)
    {
        sem_wait(&buf_idle);    //缓冲队列是否有空
        sem_wait(&mutex);       //缓冲队列是否被他人占用
        buffer[index] = 1;
        index = ((index+1)%500);
        sem_post(&buf_full);
        sem_post(&mutex);

        sleep(1);
    }
}
void p2(void *arg)      //pro
{
    while(1)
    {
        sem_wait(&buf_idle);    //缓冲队列是否有空
        sem_wait(&mutex);       //缓冲队列是否被他人占用
        buffer[index] = 2;
        index = ((index+1)%500);
        sem_post(&buf_full);
        sem_post(&mutex);

        sleep(1);        
    }
}
void p3(void *arg)      //reduce
{
    static int rct = 0;
    while(1)
    {
        if(rct == 0)
            sem_wait(&r_flag);
        sem_wait(&buf_full);
        sem_wait(&mutex);
        printf("R1 rec:%d\r\n",buffer[rindex]);
        buffer[rindex] = 0;
        rindex = ((rindex+1)%500);
        rct++;
        sem_post(&buf_idle);
        if(rct == 3)
        {
            sem_post(&r_flag);
            rct = 0;
        }
        sem_post(&mutex);

        sleep(1); 
    }  
}

void p4(void *arg)      //reduce
{
    static int rct = 0;
    while(1)
    {
        if(rct == 0)
            sem_wait(&r_flag);
        sem_wait(&buf_full);
        sem_wait(&mutex);
        printf("R2 rec:%d\r\n",buffer[rindex]);
        buffer[rindex] = 0;
        rindex = ((rindex+1)%500);
        rct++;
        sem_post(&buf_idle);
        if(rct == 3)
        {
            sem_post(&r_flag);
            rct = 0;
        }
        sem_post(&mutex);

        sleep(1); 
    }  
}
void p5(void *arg)      //reduce
{
    static int rct = 0;
    while(1)
    {
        if(rct == 0)
            sem_wait(&r_flag);
        sem_wait(&buf_full);
        
        sem_wait(&mutex);
        printf("R3 rec:%d\r\n",buffer[rindex]);
        buffer[rindex] = 0;
        rindex = ((rindex+1)%500);
        rct++;
        sem_post(&buf_idle);
        if(rct == 3)
        {
            sem_post(&r_flag);
            rct = 0;
        }
        sem_post(&mutex);   

        sleep(1);      
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
        pthread_create(&p[i].p,NULL,p[i].task,arg);
    }
    pthread_exit(NULL);
    return 0;
}

int main(int argc,char** argv)
{
    /* 信号量初始化 */
    sem_init(&mutex,0,1);       //缓冲区空闲
    sem_init(&buf_full,0,0);
    sem_init(&buf_idle,0,500);
    sem_init(&r_flag,0,1);          

    parbegin(argv[1]);

    return 0;
}