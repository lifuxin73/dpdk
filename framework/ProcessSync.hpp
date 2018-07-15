/*
 * ProcessSync.hpp
 *
 *  Created on: Jan 14, 2018
 *      Author: lifuxin
 */

#ifndef DPDK_FRAMEWORK_PROCESSSYNC_HPP_
#define DPDK_FRAMEWORK_PROCESSSYNC_HPP_

#include <semaphore.h>
#include <fcntl.h>
#include <string>
#include <stdio.h>
class SemNoName
{
public:
    SemNoName():sem(NULL){}

    virtual ~SemNoName(){}

    virtual bool CreateSem();

    bool PostSem();
    bool WaitSem();

    bool CloseSem();
    sem_t* GetSem();
protected:
    sem_t *sem;
};

sem_t* SemNoName::GetSem()
{
    return sem;
}
bool SemNoName::CreateSem()
{
	int rel = sem_init(sem, 0, 0);
    if( 0 != rel )
    {
        perror("sem_open failed");
        return false;
    }
    printf("sem_open successful!\n");
    return true;
}


bool SemNoName::PostSem()
{
    int rel = sem_post(sem);
    if( -1 == rel )
    {
        perror("sem_post failed");
        return false;
    }
    return true;
}

bool SemNoName::WaitSem()
{
    int rel = sem_wait(sem);

    printf("sem_wait : %d\n",rel);
    if( -1 == rel )
    {
        perror("sem_wait failed");
        return false;
    }
    return true;
}

bool SemNoName::CloseSem()
{
    int rel = sem_close(sem);
    if( -1 == rel )
    {
        perror("sem_close failed");
        return false;
    }
    return true;
}




class SemName : public SemNoName
{
public:
	SemName(std::string name);
	bool CreateSem();
	bool CheckSem();
	bool UnlinkSem();
private:
	std::string name;
};

SemName::SemName(std::string name): name(name){}

bool SemName::CreateSem()
{
    sem=sem_open(name.c_str(),O_CREAT,0644,0);
    if( SEM_FAILED == sem )
    {
        perror("sem_open failed");
        return false;
    }
    printf("sem_open successful %s!\n", name.c_str());
    printf("sem_open successful!\n");
    return true;
}
bool SemName::CheckSem()
{
    sem=sem_open(name.c_str(),O_CREAT|O_EXCL,0644,0);
    if(SEM_FAILED == sem)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool SemName::UnlinkSem()
{
    int rel = sem_unlink(name.c_str());
    if( -1 == rel )
    {
        perror("sem_close failed");
        return false;
    }
    return true;
}
#endif /* DPDK_FRAMEWORK_PROCESSSYNC_HPP_ */
