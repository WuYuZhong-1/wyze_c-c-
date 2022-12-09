#ifndef _THREADPOOL_H
#define _THREADPOOL_H

typedef struct ThreadPool  ThreadPool;
// 创建一个线程池
ThreadPool * threadPoolCreate(int min, int max, int queueSize);

// 摧毁一个线程池
int threadPoolDestroy(ThreadPool * pool);

// 添加任务
void threadPoolAdd( ThreadPool *pool, void (*func)(void *), void *arg);

// 获取工作中的线程个数
int threadPoolBusyNum(ThreadPool *pool);

// 获取活着的线程个数
int threadPoolAliveNum( ThreadPool *pool);



void printTest();


#endif // _THREADPOOL_H