#include "stdio.h"
#include "stdlib.h"
#include "fcntl.h"
#include "unistd.h"
#include "sys/types.h"
#include "pthread.h"
#include "signal.h"
#include "semaphore.h"
#include "time.h"

#define BUSY      1
#define SLEEP     0 

#define NUM_OF_WAITCHAR 3
/* 信号量 */
sem_t wait_char;
sem_t barb_char;
sem_t barber;//0:busy 1:sleep
sem_t client;

int wait_person;

/* 优化 */
//1.有限等待 -> sem_timedwait
struct timespec wait={
    .tv_sec=0,
    .tv_nsec = 1000*1000//1ms
};
//2.隐含着读者优先,让写者优先



/* 线程定义 */
#define PTHREAD_NUM 5    //5 pthread
struct pthread{
    pthread_t p;
    void* (*task)(void*);
};

struct pthread p[PTHREAD_NUM];

/* 理发师问题 */
void p1(void *arg)  //sleep
{
    while(1)
    {
        /* 理发师睡觉 */
        sem_getvalue(&wait_char,&wait_person);
        if(wait_person == 3)
            printf("理发师睡觉 等待客人数目%d!\r\n",3-wait_person);
        sem_wait(&barber);            //等客人叫醒
        sem_post(&wait_char);         //等候椅空闲出一把
        sem_wait(&client);            //顾客满意的结束了剪头
    }
}
void p2(void *arg)  //客人1
{
    while(1)
    {
        sem_wait(&wait_char);   //看有几人在等待，若大于3人则阻塞，小于三则人占用一把等候椅
        printf("客人 1 排队中!\r\n");
        sem_post(&barber);      //向理发师发出要剪头的信息
        sem_wait(&barb_char);   //等到理发椅，占用理发椅  
        printf("客人 1 正在剪头!\r\n");
        usleep(1000*1000);
        sem_post(&barb_char);   //理发完成
        printf("客人 1 剪头完毕!\r\n");
        sem_post(&client);      //向理发师发送完成信号
    }
}
void p3(void *arg)  //客人2
{
    while(1)
    {
        sem_wait(&wait_char);   //看有几人在等待，若大于3人则阻塞，小于三则人占用一把等候椅
        printf("客人 2 排队中!\r\n");
        sem_post(&barber);      //向理发师发出要剪头的信息
        sem_wait(&barb_char);   //等到理发椅，占用理发椅  
        printf("客人 2 正在剪头!\r\n");
        usleep(1000*1000);
        sem_post(&barb_char);   //理发完成
        printf("客人 2 剪头完毕!\r\n");
        sem_post(&client);      //向理发师发送完成信号
    }  
}
void p4(void *arg)  //客人3
{
    while(1)
    {
        sem_wait(&wait_char);   //看有几人在等待，若大于3人则阻塞，小于三则人占用一把等候椅
        printf("客人 3 排队中!\r\n");
        sem_post(&barber);      //向理发师发出要剪头的信息
        sem_wait(&barb_char);   //等到理发椅，占用理发椅  
        printf("客人 3 正在剪头!\r\n");
        usleep(1000*1000);
        sem_post(&barb_char);   //理发完成
        printf("客人 3 剪头完毕!\r\n");
        sem_post(&client);      //向理发师发送完成信号
    }  
}
void p5(void *arg)  //客人4
{
    while(1)
    {
        sem_wait(&wait_char);   //看有几人在等待，若大于3人则阻塞，小于三则人占用一把等候椅
        printf("客人 4 排队中!\r\n");
        sem_post(&barber);      //向理发师发出要剪头的信息
        sem_wait(&barb_char);   //等到理发椅，占用理发椅  
        printf("客人 4 正在剪头!\r\n");
        usleep(1000*1000);
        sem_post(&barb_char);   //理发完成
        printf("客人 4 剪头完毕!\r\n");
        sem_post(&client);      //向理发师发送完成信号
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
    
    sem_init(&barber,0,SLEEP);
    sem_init(&barb_char,0,1);
    sem_init(&wait_char,0,NUM_OF_WAITCHAR);
    sem_init(&client,0,0);

    parbegin(argv[1]);

    return 0;
}
