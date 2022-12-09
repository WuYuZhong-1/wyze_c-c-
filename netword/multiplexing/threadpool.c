#include  "threadpool.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

//任务队列
// 工作的线程
// 管理者线程

//任务结构体
typedef struct Task 
{
    void (*function)(void *arg);
    void * data;
} Task;


//线程池结构体
struct ThreadPool 
{
    //任务队列
    Task *taskQueue;                 //任务队列的存储地址
    int queueCapacity;              //任务队列的容量
    int queueCurrSize;              //当前任务个数
    int queueFront;                    //队头 -> 取数据
    int queueRear;                      //队尾  -> 放数据

    //管理者线程
    pthread_t managerID;        //管理者线程

    //工作的线程
    pthread_t *threadIDs;            //工作线程的存储地址
    int minNum;                             //最小的线程数
    int maxNum;                             //最大的线程数
    int busyNum;                            //忙碌的线程个数
    int liveNum;                                //存活线程个数
    int exitNum;                             //要摧毁的线程个数

    //数据同步
    pthread_mutex_t mutexPool;  //整个线程池
    pthread_mutex_t mutexBusy; //锁的是忙碌线程的个数

    //阻塞等待，条件变量进行阻塞
    pthread_cond_t  condNotFull ;                             //任务队列是不是满了
    pthread_cond_t condNotEmpty;                             //任务队列是不是空了

    //线程池是否要销毁
    int shutdown; //要不要销毁线程池 1为销毁 0 为不销毁
};

static void* workerStart(void *arg);
//线程退出
static void threadExit(ThreadPool *pool);

const int NUMBER = 2; // 每次添加的线程个数
static void* managerStart(void *arg) 
{
    ThreadPool *pool = (ThreadPool *)arg;

    //主要功能是根据任务调节存活的线程数
    while(!pool->shutdown){
        //每隔三秒一次
        sleep(3);

        //取出当前任务数和存活线程数
        pthread_mutex_lock(&pool->mutexPool);
        int queueSize = pool->queueCurrSize;
        int liveNum = pool->liveNum;
        pthread_mutex_unlock(&pool->mutexPool);

        //取出忙碌的线程个数
        pthread_mutex_lock(&pool->mutexBusy);
        int busyNum = pool->busyNum;
        pthread_mutex_unlock(&pool->mutexBusy);

        //添加线程
        //理论依据：当前任务个数 > 存活的线程数&& 存活的线程数  <  最大线程数
        if( queueSize > (liveNum-busyNum) && liveNum < pool->maxNum) {
            
            pthread_mutex_lock(&pool->mutexPool);
            int counter =0;
            for(int i =0; i < pool->maxNum && counter < NUMBER && pool->liveNum < pool->maxNum; i++) {

                if(pool->threadIDs[i] == 0) {

                    if(pthread_create(&pool->threadIDs[i], NULL, workerStart, pool)) {
                        printf("manager add thread failed\n");
                        break;
                    } 
                    counter++;
                    pool->liveNum++;
                }
            }

            pthread_mutex_unlock(&pool->mutexPool);
        }

        //销毁线程
        //理论依据: 忙的线程数 × 2  < 存活的线程数 && 存活的线程数 > 最小线程数
        if( busyNum * 2 < liveNum && liveNum > pool->minNum) {
            pthread_mutex_lock(&pool->mutexPool);
            //避免存活的线程数小于最小线程数
            int exitNum = liveNum - pool->minNum;
            exitNum = exitNum < NUMBER ?  exitNum : NUMBER;
            pool->exitNum = exitNum;
            pthread_mutex_unlock(&pool->mutexPool);
            //管理者线程不宜杀死工作的线程，应该让工作的线程自杀，传递一个自杀任务，使存活等待的线程自杀
            for(int i =0; i < exitNum; i++) {
                pthread_cond_signal(&pool->condNotEmpty);
            }
        }
    }
    pthread_exit(NULL);
}

static void* workerStart(void *arg)
{
    ThreadPool *pool = (ThreadPool *)arg;

    while(1) {

        //枷锁
        pthread_mutex_lock(&pool->mutexPool);

        //这里不能用if，而是用while，防止被唤醒后拿不到数据
        // if(pool->queueCurrSize == 0) {
        //     pthread_cond_wait(&pool->condNotEmpty, &pool->mutexPool);
        // }
        while(pool->queueCurrSize ==  0 && pool->shutdown == 0) {
            pthread_cond_wait(&pool->condNotEmpty, &pool->mutexPool);

            //判断是否要进行自杀
            if(pool->exitNum > 0) {
                pool->exitNum--;
                
                if(pool->liveNum > pool->minNum) {
                    pool->liveNum--;
                    pthread_mutex_unlock(&pool->mutexPool);
                    threadExit(pool);
                }
            }
        }

        //线程退出
        if(pool->shutdown == 1) {
            pthread_mutex_unlock(&pool->mutexPool);
            threadExit(pool);
        }

        //获取数据
        Task task = pool->taskQueue[pool->queueFront];
        pool->taskQueue[pool->queueFront].function = NULL;
        pool->taskQueue[pool->queueFront].data = NULL;
        pool->queueCurrSize--;

        //移动数据头
        pool->queueFront = (pool->queueFront + 1) % pool->queueCapacity;

        //换醒生产者
        pthread_cond_signal(&pool->condNotFull);
        //解锁
        pthread_mutex_unlock(&pool->mutexPool);

        //设置忙碌的线程
        pthread_mutex_lock(&pool->mutexBusy);
        printf("thread %ld start working\n", pthread_self());
        pool->busyNum++;
        pthread_mutex_unlock(&pool->mutexBusy);

        //执行任务
        task.function(task.data);
        //这里的释放是要约定好的
        if(task.data != NULL) {
            free(task.data);
            task.data = NULL;
        }

        //设置线程忙碌减少
        pthread_mutex_lock(&pool->mutexBusy);
        printf("thread %ld end working\n",pthread_self());
        pool->busyNum--;
        pthread_mutex_unlock(&pool->mutexBusy);
    }
}


ThreadPool *threadPoolCreate(int min, int max, int queueSize)
{
    ThreadPool *pool =  (ThreadPool *)malloc(sizeof(ThreadPool));
        if(pool == NULL) {
            printf("malloc pool failed\n");
            return NULL;
        }
        memset(pool, 0, sizeof(ThreadPool));

    //系统资源标志为，为了分配不成功释放资源
    int mutexPoolFlag = 0;
    int mutexBusyFlag = 0;
    int condNotEmptyFlag = 0;
    int condNotFullFlag = 0;
    int managerIDFlag = 0;
    int workerCountFlag = 0;

    do {
        
        //工作线程处理
        pool->threadIDs = (pthread_t *) malloc(sizeof(pthread_t) * max);
        if(pool->threadIDs == NULL) {
            printf("malloc threadIDs failed\n");
            break;
        }
        memset(pool->threadIDs, 0, sizeof(pthread_t) * max);
        pool->minNum = min;
        pool->maxNum = max;
        pool->busyNum = 0;
        pool->liveNum = min;            //刚开始创建的线程个数按照最小线程来创建
        pool->exitNum = 0;

        //处理系统资源
        if( pthread_mutex_init(&pool->mutexPool, NULL)){
            printf("init mutexPool failed\n");
            break;
        }
        mutexPoolFlag = 1;

        if( pthread_mutex_init(&pool->mutexBusy, NULL)) {
            printf("init mutexBusy failed\n");
            break;
        }
        mutexBusyFlag = 1;

        if( pthread_cond_init(&pool->condNotEmpty, NULL) ) {
            printf("init condNotEmpty failed\n");
            break;
        }
        condNotEmptyFlag = 1;

        if(pthread_cond_init(&pool->condNotFull, NULL) ) {
            printf("init condNotFull failed\n");
            break;
        }
        condNotFullFlag = 1;

        //处理任务
        pool->taskQueue = (Task *) malloc(sizeof(Task) * queueSize);
        if(pool->taskQueue == NULL) {
            printf("malloc taskQueue failed\n");
            break;
        }
        memset(pool->taskQueue, 0, sizeof(Task));
        pool->queueCapacity = queueSize;
        pool->queueCurrSize = 0;
        pool->queueFront = 0;
        pool->queueRear = 0;

        pool->shutdown = 0;

        //创建线程 管理者线程
        if(pthread_create(&pool->managerID, NULL,managerStart, pool) ) {
            printf("create managerStart failed\n");
            break;
        }
        managerIDFlag = 1;

        //创建工作的线程
        for(; workerCountFlag < pool->minNum; workerCountFlag++) {
            if(pthread_create(&(pool->threadIDs[workerCountFlag]), NULL, workerStart, (void *)pool) ) {
                printf("init %d worker failed\n",workerCountFlag);
                break;
            }
        }

        if(workerCountFlag < pool->minNum)      //判断创建的工作数是否正确，小于表示创建的工作数小了
            break;

        return pool;
    } while(0);
    
    pool->shutdown = 1;
    // workerCountFlag -1 表示之前的线程是创建成功
    for(int i = workerCountFlag -1; i < 0; i-- ) {
	pthread_cond_signal(&pool->condNotEmpty);
    }

    if(managerIDFlag == 1){
        pthread_cancel(pool->managerID);
        pthread_join(pool->managerID, NULL);
    }

    if(pool->taskQueue != NULL)
        free(pool->taskQueue);

    if(condNotFullFlag == 1) 
        pthread_cond_destroy(&pool->condNotFull);
    
    if(condNotEmptyFlag == 1)
        pthread_cond_destroy(&pool->condNotEmpty);

    if(mutexBusyFlag == 1 )
        pthread_mutex_destroy(&pool->mutexBusy);

    if(mutexPoolFlag == 1) 
        pthread_mutex_destroy(&pool->mutexPool);
    
    if(pool->threadIDs != NULL) 
        free(pool->threadIDs);
    
    //释放资源
    if(pool != NULL ) 
        free(pool);
    
    return NULL;
}


static void threadExit(ThreadPool *pool)
{
    pthread_t tid  = pthread_self();
    pthread_mutex_lock(&pool->mutexPool);
    for(int i =0; i < pool->maxNum; i++) {
        if(pool->threadIDs[i] == tid) {
            pool->threadIDs[i] = 0;
            break;
        }
    }
    pthread_mutex_unlock(&pool->mutexPool);
    pthread_exit(NULL);
}


void threadPoolAdd( ThreadPool *pool, void (*func)(void *), void *data)
{
    if( pool == NULL)
        return;
    pthread_mutex_lock(&pool->mutexPool);

    //判断队列是否满了，满了则阻塞
    while(pool->queueCurrSize == pool->queueCapacity && !pool->shutdown) {
        pthread_cond_wait(&pool->condNotFull, &pool->mutexPool);
    }

    if(pool->shutdown == 1) {
        pthread_mutex_unlock(&pool->mutexPool);
        return;
    }

    //添加任务
    Task task = { .function = func, .data= data};

    //往队列尾巴添加
    pool->taskQueue[pool->queueRear] = task;
    pool->queueRear = (pool->queueRear +1) % pool->queueCapacity;
    pool->queueCurrSize++;

    //通知线程可以进行工作
    pthread_cond_signal(&pool->condNotEmpty);
    pthread_mutex_unlock(&pool->mutexPool);
}

int threadPoolBusyNum(ThreadPool *pool)
{
    if(pool == NULL)
        return -1;

    pthread_mutex_lock(&pool->mutexBusy);
    int busyNum =  pool->busyNum;
    pthread_mutex_unlock(&pool->mutexBusy);
    return busyNum;
}

int threadPoolAliveNum( ThreadPool *pool)
{
    if (pool == NULL)
        return -1;

    pthread_mutex_lock(&pool->mutexPool);
    int aliveNum = pool->liveNum;
    pthread_mutex_unlock(&pool->mutexPool);
    return aliveNum;
}


int threadPoolDestroy(ThreadPool * pool)
{
    if(pool == NULL)
        return -1;

    //关闭线程池
    pthread_mutex_lock(&pool->mutexPool);
    pool->shutdown = 1;
    pthread_mutex_unlock(&pool->mutexPool);

    //回收管理者线程
    pthread_join(pool->managerID, NULL);
    //回收工作者线程
    for(int i =0; i < pool->liveNum; i++) {
        pthread_cond_signal(&pool->condNotEmpty);
    }

    //释放资源
    pthread_mutex_destroy(&pool->mutexBusy);
    pthread_mutex_destroy(&pool->mutexPool);
    pthread_cond_destroy(&pool->condNotEmpty);
    pthread_cond_destroy(&pool->condNotFull);

    if(pool->threadIDs != NULL) {
        free(pool->threadIDs);
        pool->threadIDs = NULL;
    }
    
    if(pool->taskQueue) {
        free(pool->taskQueue);
        pool->taskQueue = NULL;
    }
        
    free(pool);
    pool = NULL;
    
    return 0;
}














void printTest()
{
    printf("threadpool test\n");
}
