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
sem_t reduceAB;
sem_t reduceBA;
sem_t item;

int m;
int n;


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
void p1(void *arg)      //proA
{
    while(1)
    {
        sem_wait(&mutex);//缓冲队列是否被他人占用
        buffer[index] = 1;
        index = ((index+1)%500);
        sem_post(&item);
        sem_post(&mutex);
        sem_post(&reduceAB);
        sem_wait(&reduceBA);


        sleep(1);
    }
}
void p2(void *arg)      //proB
{
    while(1)
    {
        sem_wait(&mutex);       //缓冲队列是否被他人占用
        buffer[index] = 2;
        index = ((index+1)%500);
        sem_post(&item);
        sem_post(&mutex);
        sem_wait(&reduceAB);
        sem_post(&reduceBA);


        usleep(500*1000);
        //sleep(1);        
    }
}
void p3(void *arg)      //dealerC
{
    int AreduceB,BreduceA;
    while(1)
    {
        sem_wait(&item);
        sem_wait(&mutex);
        sem_getvalue(&reduceAB,&AreduceB);
        sem_getvalue(&reduceBA,&BreduceA);
        printf("dealer :%d\t",buffer[rindex]);
        printf("A - B = %d\tB - A = %d\r\n",m-BreduceA,n-AreduceB);
        rindex = ((rindex+1)%500);
        sem_post(&mutex);

        //sleep(1);
    }  
}
/*
void p4(void *arg)      //reduce
{
    while(1)
    {
        
    }  
}
void p5(void *arg)      //reduce
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
    printf("please input m && n!\r\n");
    scanf("%d%d",&m,&n);
    printf("m:%d  n:%d\r\n",m,n);
    sleep(1);
    system("/home/xny/shell/clr.sh");

    sem_init(&mutex,0,NON_BUSY);       //缓冲区空闲
    sem_init(&reduceAB,0,n-1);
    sem_init(&reduceBA,0,m-1);
    sem_init(&item,0,0);

    parbegin(argv[1]);

    return 0;
}