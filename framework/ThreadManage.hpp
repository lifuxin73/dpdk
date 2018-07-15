/*
 * ThreadManage.hpp
 *
 *  Created on: Jan 14, 2018
 *      Author: lifuxin
 */

#ifndef DPDK_FRAMEWORK_THREADMANAGE_HPP_
#define DPDK_FRAMEWORK_THREADMANAGE_HPP_

#include<pthread.h>
#include<boost/smart_ptr.hpp>

class Task_thread
{
public:
    virtual void start(){};
    virtual ~Task_thread(){};
private:
};
class Thread
{
public:
    Thread(pthread_t tid);
    pthread_t GetTid();
    void Start();
    void AddTask(Task_thread* task);
    void StartTask();
    void run();
private:
    bool isexit;
    pthread_t tid;
    pthread_cond_t cond;
    pthread_mutex_t lock;
    Task_thread* task;
};

Thread::Thread(pthread_t tid): isexit(false), tid(tid), task(NULL)
{
    int rel = pthread_cond_init(&cond, NULL);
    if(0 != rel)
    {
        printf("pthread_cond_init fail");
    }
    rel = pthread_mutex_init(&lock, NULL);
    if(0 != rel)
    {
        printf("pthread_mutex_init fail");
    }

}

pthread_t  Thread::GetTid()
{
    return tid;
}

void Thread::Start()
{
    while(!isexit)
    {
        pthread_cond_wait(&cond, &lock);
        run();
    }
}

void Thread::AddTask(Task_thread* _task)
{
    task = _task;
}

void Thread::StartTask()
{
    pthread_cond_signal(&cond);
}

void Thread::run()
{
    task->start();
}

class ThreadManage
{
public:
    bool CreateThread();
    static void* function(void *arg);
    std::vector<boost::shared_ptr<Thread> > threads;
};

void* ThreadManage::function(void *arg)
{
    pthread_t tid;
    ThreadManage* threadManage = (ThreadManage*)arg;
    tid = pthread_self();
    boost::shared_ptr<Thread> thread(new Thread(tid));
    threadManage->threads.push_back(thread);

    thread->Start();

    return NULL;
}

bool ThreadManage::CreateThread()
{
    pthread_t thr;
    if(pthread_create(&thr, NULL, ThreadManage::function, this)!=0)
    {
        printf("create thread failed!\n");
        return false;
    }
    return true;
}
#endif /* DPDK_FRAMEWORK_THREADMANAGE_HPP_ */
