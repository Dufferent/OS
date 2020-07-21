#include "stdio.h"
#include "stdlib.h"
#include "fcntl.h"
#include "unistd.h"
#include "sys/types.h"
#include "pthread.h"
#include "signal.h"
#include "semaphore.h"
#include "time.h"

#define BUFFER_TYPE int
#define BUFF_LEN 10
#define NON_BUSY 1
#define BUSY     0
#define FULL     10
#define EMPTY    0
/* 缓冲池 */
BUFFER_TYPE buffer[BUFF_LEN] = {0};
BUFFER_TYPE* pp;
BUFFER_TYPE* rp;

unsigned int ppos = 0;
unsigned int rpos = 0;
/* 信号量（优化版，有让权等待） */
sem_t buff_rw_state;
sem_t idle;
sem_t full;

struct timespec wait={
    .tv_sec=0,
    .tv_nsec = 1000*1000//1ms
};



/* 线程定义 */
#define PTHREAD_NUM 10    //5 pro : 5 red
struct product{
    pthread_t pro;
    void* (*task)(void*);
};
struct reducer{
    pthread_t red;
    void* (*task)(void*);
};
//pthread_t product[PTHREAD_NUM/2];
//pthread_t reducer[PTHREAD_NUM/2];
struct product p[PTHREAD_NUM/2];
struct reducer r[PTHREAD_NUM/2];

unsigned int pcnt = 0;
unsigned int rcnt = 0;
/* 生产者 */
void p1(void *arg)
{
    //printf("p1 run!\r\n");
    static int val = 1;
    while(1)
    {
        sem_timedwait(&buff_rw_state,&wait);//看看缓冲区读写状态
        sem_timedwait(&idle,&wait);         //空闲的缓冲区如果之前已经为0，调用wait后为负数，则阻塞，等待空闲   
        /* 临界区 */
        buffer[ppos] = val;
        //printf("放入:%d\r\n",buffer[ppos]);
        ppos = ((ppos+1)%BUFF_LEN);
        //*pp = val;
        //pp++;
        //pcnt++;
        //if(pcnt == BUFF_LEN)
        //{
        //    pp=buffer;
        //    pcnt = 0;
        //}
        sem_post(&full);
        sem_post(&buff_rw_state);
        /* 剩余区 */
        //usleep(1*1000);
    }
}
void p2(void *arg)
{
    //printf("p2 run!\r\n");
    static int val = 2;
    while(1)
    {
        sem_timedwait(&buff_rw_state,&wait);//看看缓冲区读写状态
        sem_timedwait(&idle,&wait);         //空闲的缓冲区如果之前已经为0，调用wait后为负数，则阻塞，等待空闲   
        /* 临界区 */
        buffer[ppos] = val;
        //printf("放入:%d\r\n",buffer[ppos]);
        ppos = ((ppos+1)%BUFF_LEN);
        //*pp = val;
        //pp++;
        //pcnt++;
        //if(pcnt == BUFF_LEN)
        //{
        //    pp=buffer;
        //    pcnt = 0;
        //}
        sem_post(&full);
        sem_post(&buff_rw_state);
        /* 剩余区 */
        //usleep(2*1000);
    }
}
void p3(void *arg)
{
    //printf("p3 run!\r\n");  
    static int val = 3;
    while(1)
    {
        sem_timedwait(&buff_rw_state,&wait);//看看缓冲区读写状态
        sem_timedwait(&idle,&wait);         //空闲的缓冲区如果之前已经为0，调用wait后为负数，则阻塞，等待空闲   
        /* 临界区 */
        buffer[ppos] = val;
        //printf("放入:%d\r\n",buffer[ppos]);
        ppos = ((ppos+1)%BUFF_LEN);
        //*pp = val;
        //pp++;
        //pcnt++;
        //if(pcnt == BUFF_LEN)
        //{
        //    pp=buffer;
        //    pcnt = 0;
        //}
        sem_post(&full);
        sem_post(&buff_rw_state);
        /* 剩余区 */
        //usleep(3*1000);
    }  
}
void p4(void *arg)
{
    //printf("p4 run!\r\n");  
    static int val = 4;
    while(1)
    {
        sem_timedwait(&buff_rw_state,&wait);//看看缓冲区读写状态
        sem_timedwait(&idle,&wait);         //空闲的缓冲区如果之前已经为0，调用wait后为负数，则阻塞，等待空闲   
        /* 临界区 */
        buffer[ppos] = val;
        //printf("放入:%d\r\n",buffer[ppos]);
        ppos = ((ppos+1)%BUFF_LEN);
        //*pp = val;
        //pp++;
        //pcnt++;
        //if(pcnt == BUFF_LEN)
        //{
        //    pp=buffer;
        //    pcnt = 0;
        //}
        sem_post(&full);
        sem_post(&buff_rw_state);
        /* 剩余区 */
        //usleep(5*1000);
    }  
}
void p5(void *arg)
{
    //printf("p5 run!\r\n");  
    static int val = 5;
    while(1)
    {
        sem_timedwait(&buff_rw_state,&wait);//看看缓冲区读写状态
        sem_timedwait(&idle,&wait);         //空闲的缓冲区如果之前已经为0，调用wait后为负数，则阻塞，等待空闲   
        /* 临界区 */
        buffer[ppos] = val;
        //printf("放入:%d\r\n",buffer[ppos]);
        ppos = ((ppos+1)%BUFF_LEN);
        //*pp = val;
        //pp++;
        //pcnt++;
        //if(pcnt == BUFF_LEN)
        //{
        //    pp=buffer;
        //    pcnt = 0;
        //}
        sem_post(&full);
        sem_post(&buff_rw_state);
        /* 剩余区 */
        //usleep(7*1000);
    }  
}
/* 消费者 */
void r1(void *arg)
{
    //printf("r1 run!\r\n");
    while(1)
    {
        sem_timedwait(&buff_rw_state,&wait);//看看缓冲区读写状态
        sem_timedwait(&full,&wait);         //占用的缓冲区如果之前已经为0，调用wait后为负数，则阻塞，等待发货   
        /* 临界区 */
        printf("取走:%d\r\n",buffer[rpos]);
        rpos = ((rpos+1)%BUFF_LEN);
        //*rp = 0;
        //rp++;
        //rcnt++;
        //if(rcnt == BUFF_LEN)
        //{
        //    pp=buffer;
        //    rcnt = 0;
        //}
        sem_post(&idle);        
        sem_post(&buff_rw_state);
        /* 剩余区 */
        //usleep(11*1000);
    }
}
void r2(void *arg)
{
    //printf("r2 run!\r\n");
    while(1)
    {
        sem_timedwait(&buff_rw_state,&wait);//看看缓冲区读写状态
        sem_timedwait(&full,&wait);         //占用的缓冲区如果之前已经为0，调用wait后为负数，则阻塞，等待发货   
        /* 临界区 */
        printf("取走:%d\r\n",buffer[rpos]);
        rpos = ((rpos+1)%BUFF_LEN);
        //*rp = 0;
        //rp++;
        //rcnt++;
        //if(rcnt == BUFF_LEN)
        //{
        //    pp=buffer;
        //    rcnt = 0;
        //}
        sem_post(&idle);        
        sem_post(&buff_rw_state);
        /* 剩余区 */
        //usleep(13*1000);
    }
}
void r3(void *arg)
{
    //printf("r3 run!\r\n");
    while(1)
    {
        sem_timedwait(&buff_rw_state,&wait);//看看缓冲区读写状态
        sem_timedwait(&full,&wait);         //占用的缓冲区如果之前已经为0，调用wait后为负数，则阻塞，等待发货   
        /* 临界区 */
        printf("取走:%d\r\n",buffer[rpos]);
        rpos = ((rpos+1)%BUFF_LEN);
        //*rp = 0;
        //rp++;
        //rcnt++;
        //if(rcnt == BUFF_LEN)
        //{
        //    pp=buffer;
        //    rcnt = 0;
        //}
        sem_post(&idle);        
        sem_post(&buff_rw_state);
        /* 剩余区 */
        //usleep(16*1000);
    }
}
void r4(void *arg)
{
    //printf("r4 run!\r\n");
    while(1)
    {
        sem_timedwait(&buff_rw_state,&wait);//看看缓冲区读写状态
        sem_timedwait(&full,&wait);         //占用的缓冲区如果之前已经为0，调用wait后为负数，则阻塞，等待发货   
        /* 临界区 */
        printf("取走:%d\r\n",buffer[rpos]);
        rpos = ((rpos+1)%BUFF_LEN);
        //*rp = 0;
        //rp++;
        //rcnt++;
        //if(rcnt == BUFF_LEN)
        //{
        //    pp=buffer;
        //    rcnt = 0;
        //}
        sem_post(&idle);        
        sem_post(&buff_rw_state);
        /* 剩余区 */
        //usleep(17*1000);
    }
}
void r5(void *arg)
{
    //printf("r5 run!\r\n");
    while(1)
    {
        sem_timedwait(&buff_rw_state,&wait);//看看缓冲区读写状态
        sem_timedwait(&full,&wait);         //占用的缓冲区如果之前已经为0，调用wait后为负数，则阻塞，等待发货   
        /* 临界区 */
        printf("取走:%d\r\n",buffer[rpos]);
        rpos = ((rpos+1)%BUFF_LEN);
        //*rp = 0;
        //rp++;
        //rcnt++;
        //if(rcnt == BUFF_LEN)
        //{
        //    pp=buffer;
        //    rcnt = 0;
        //}
        sem_post(&idle);        
        sem_post(&buff_rw_state);
        /* 剩余区 */
        //usleep(19*1000);
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

    r[0].task = (void*)r1;
    r[1].task = (void*)r2;
    r[2].task = (void*)r3;
    r[3].task = (void*)r4;
    r[4].task = (void*)r5;


    for(int i=0;i<PTHREAD_NUM/2;i++)
    {
        pthread_create(&p[i].pro,NULL,p[i].task,arg);//生产者
        pthread_create(&r[i].red,NULL,r[i].task,arg);//消费者
    }
    pthread_exit(NULL);
    return 0;
}

int main(int argc,char** argv)
{
    /* 生产者 消费者指针都指向缓冲区的首地址 */
    pp = rp = buffer;

    /* 信号量初始化 */
    sem_init(&buff_rw_state,0,NON_BUSY); //缓冲区闲置
    sem_init(&idle,0,FULL);              //空闲的缓冲区
    sem_init(&full,0,EMPTY);             //占用的缓冲区

    parbegin(argv[1]);

    return 0;
}
