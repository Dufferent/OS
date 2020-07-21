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
sem_t terminal;//终端
sem_t machine; //机器
sem_t patient_num;//报到人数
sem_t docter;  //医生
sem_t illness; //病人




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
void p1(void *arg)      //docter
{
    int num;
    while(1)
    {
        sem_getvalue(&patient_num,&num);
        if(num == 0)
            printf("docter is sleeping!\r\n");

        sem_wait(&patient_num);//等待病人报到
        sem_wait(&illness);
    }
}
void p2(void *arg)      //patient 1
{
    while(1)
    {
        sem_wait(&terminal);//拿号
        printf("1 号病人拿号!\r\n");
        sem_post(&terminal);

        sem_wait(&machine);//报到
        printf("1 号病人报到!\r\n");
        sem_post(&machine);

        sem_post(&patient_num);

        sem_wait(&docter);//医生是不是有空
        printf("1 号病人看病!\r\n");
        sleep(1);//看了这么久
        sem_post(&docter);  //看完病离开医生办公室
        sem_post(&illness);
        printf("1 号病人看完病离开医生办公室!\r\n");
        printf("1 号病人拿药离开!\r\n");
    }
}
void p3(void *arg)      //patient 2
{
    while(1)
    {
        sem_wait(&terminal);//拿号
        printf("2 号病人拿号!\r\n");
        sem_post(&terminal);

        sem_wait(&machine);//报到
        printf("2 号病人报到!\r\n");
        sem_post(&machine);

        sem_post(&patient_num);

        sem_wait(&docter);//医生是不是有空
        printf("2 号病人看病!\r\n");
        sleep(1);//看了这么久
        sem_post(&docter);//看完病离开医生办公室
        sem_post(&illness);
        printf("2 号病人看完病离开医生办公室!\r\n");
        printf("2 号病人拿药离开!\r\n");
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
    sem_init(&docter,0,1);      //医生一开始空闲，睡觉
    sem_init(&terminal,0,1);    //终端拿号也空闲
    sem_init(&machine,0,1);     //机器也空着
    sem_init(&patient_num,0,0); //报道人数0
    sem_init(&illness,0,0);
    parbegin(argv[1]);

    return 0;
}