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

/* 信号量 */
sem_t stick[5];//五支筷子
/* 优化 */
//1.有限等待 -> sem_timedwait
struct timespec wait={
    .tv_sec=0,
    .tv_nsec = 1000*1000//1ms
};
//2.限制最大同时拿筷子的人数
sem_t get_stick;
//sem_init(&get_stick,0,4);

//3.判断两侧筷子都可拿，才去拿筷子
//sem_getvalue(&s,val);
//4.奇数位哲学家拿右边的筷子，偶数位拿左边的
//...
/* 线程定义 */
#define PTHREAD_NUM 5    //5 person
struct person{
    pthread_t man;
    void* (*task)(void*);
};

struct person p[PTHREAD_NUM];

/* 哲学家 */
void p1(void *arg)
{
    while(1)
    {
        printf("man 1 is considering!\r\n");
        usleep(500*1000);//思考500毫秒,饥饿
        /* 查看相邻的筷子 */
        sem_wait(&get_stick);
        sem_wait(&stick[0]);//筷子0有没有人用啊
        sem_wait(&stick[1]);//筷子1有没有人用啊
        /* 临界区 */
        printf("man 1 was feed!\r\n");
        sem_post(&stick[0]);
        sem_post(&stick[1]);
        sem_post(&get_stick);
        /* 放下两双筷子了 */
    }
}
void p2(void *arg)
{
    while(1)
    {
        printf("man 2 is considering!\r\n");
        usleep(500*1000);//思考500毫秒,饥饿
        /* 查看相邻的筷子 */
        sem_wait(&get_stick);
        sem_wait(&stick[1]);//筷子0有没有人用啊
        sem_wait(&stick[2]);//筷子1有没有人用啊
        /* 临界区 */
        printf("man 2 was feed!\r\n");
        sem_post(&stick[1]);
        sem_post(&stick[2]);
        sem_post(&get_stick);
        /* 放下两双筷子了 */
    }
}
void p3(void *arg)
{
    while(1)
    {
        printf("man 3 is considering!\r\n");
        usleep(500*1000);//思考500毫秒,饥饿
        /* 查看相邻的筷子 */
        sem_wait(&get_stick);
        sem_wait(&stick[2]);//筷子0有没有人用啊
        sem_wait(&stick[3]);//筷子1有没有人用啊
        /* 临界区 */
        printf("man 3 was feed!\r\n");
        sem_post(&stick[2]);
        sem_post(&stick[3]);
        sem_post(&get_stick);
        /* 放下两双筷子了 */
    }  
}
void p4(void *arg)
{
    while(1)
    {
        printf("man 4 is considering!\r\n");
        usleep(500*1000);//思考500毫秒,饥饿
        /* 查看相邻的筷子 */
        sem_wait(&get_stick);
        sem_wait(&stick[3]);//筷子0有没有人用啊
        sem_wait(&stick[4]);//筷子1有没有人用啊
        /* 临界区 */
        printf("man 4 was feed!\r\n");
        sem_post(&stick[3]);
        sem_post(&stick[4]);
        sem_post(&get_stick);
        /* 放下两双筷子了 */
    }  
}
void p5(void *arg)
{
    while(1)
    {
        printf("man 5 is considering!\r\n");
        usleep(500*1000);//思考500毫秒,饥饿
        /* 查看相邻的筷子 */
        sem_wait(&get_stick);
        sem_wait(&stick[4]);//筷子0有没有人用啊
        sem_wait(&stick[0]);//筷子1有没有人用啊
        /* 临界区 */
        printf("man 5 was feed!\r\n");
        sem_post(&stick[4]);
        sem_post(&stick[0]);
        sem_post(&get_stick);
        /* 放下两双筷子了 */
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
        pthread_create(&p[i].man,NULL,p[i].task,arg);//哲学家
    }
    pthread_exit(NULL);
    return 0;
}

int main(int argc,char** argv)
{
    /* 信号量初始化 */
    for(int i=0;i<PTHREAD_NUM;i++)
        sem_init(&stick[i],0,NON_BUSY);//筷子的初始状态都是空闲没人用

    sem_init(&get_stick,0,4);

    parbegin(argv[1]);

    return 0;
}
