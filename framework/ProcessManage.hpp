/*
 * ProcessManage.hpp
 *
 *  Created on: Jan 3, 2018
 *      Author: lifuxin
 */

#ifndef DPDK_FRAMEWORK_PROCESSMANAGE_HPP_
#define DPDK_FRAMEWORK_PROCESSMANAGE_HPP_
#include<unistd.h>

#include <signal.h>
#include <assert.h>
#include<stdio.h>
#include<stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include<sys/types.h>
#include<sys/param.h>
#include <vector>
#include<boost/smart_ptr.hpp>

#include<iostream>
class Process
{
public:
	Process(pid_t pid);
	pid_t getPid();
private:
	pid_t fpid;
};

pid_t Process::getPid()
{
	return fpid;
}

Process::Process(pid_t pid):fpid(pid)
{}

class ProcessManage
{
public:
	ProcessManage(int coreCounter);
	bool CreateChilden(ManageDLL* manageDLL);
	std::vector<boost::shared_ptr<Process> >& getPrecesses();
	void waitProcess();
	static void init_daemon();
private:
	std::vector<boost::shared_ptr<Process> >::iterator findPos(pid_t pid);
	bool SetAffinity(int core);
	int coreCounter;
	std::vector<boost::shared_ptr<Process> > precesses;
};

void ProcessManage::init_daemon()
{
    int pid;
    int i;
    pid=fork();
    if( pid )
    {
        exit(0);
    }
    else if(pid< 0)
    {
        exit(1);
    }

    setsid();

    pid = fork();
    if(pid)
    {
        exit(0);
    }
    else if(pid< 0)
    {
        exit(1);
    }

    for(i=0;i< NOFILE;++i)
    {
        close(i);
    }

    chdir("/tmp");

    umask(0);
    return;
}

void ProcessManage::waitProcess()
{
    pid_t pid = wait(NULL);
    std::vector<boost::shared_ptr<Process> >::iterator it = findPos(pid);

    if(it == precesses.end())
    {
        printf("unknown process exit id : %d\n",pid);
        perror("unknown precess exit.\n");
    }

    printf("process exit id : %d\n",(*it)->getPid());
    precesses.erase(it);

    for(int index = 0; index < getPrecesses().size(); index++)
    {
        kill(getPrecesses()[index]->getPid(),SIGKILL);
        printf("kill process id : %d\n",getPrecesses()[index]->getPid());
    }
}

std::vector<boost::shared_ptr<Process> >::iterator ProcessManage::findPos(pid_t pid)
{
	std::vector<boost::shared_ptr<Process> >::iterator it = precesses.begin();

	for(it = precesses.begin(); it != precesses.end(); it++)
	{
		if((*it)->getPid() == pid)
		{
			return it;
		}
	}
	return it;
}


std::vector<boost::shared_ptr<Process> >& ProcessManage::getPrecesses()
{
	return precesses;
}

ProcessManage::ProcessManage(int coreCounter):coreCounter(coreCounter){}

bool ProcessManage::CreateChilden(ManageDLL* manageDLL)
{

    for(int index = 1; index < coreCounter ;index++)
    {
        pid_t pid = fork();
        if(pid == 0)
        {
            bool result = SetAffinity(index);
            assert(result);
            if(index == 5)
            {
                sleep(2);
                abort();
            }

            boost::shared_ptr<DllInfo> dll = ManageDLL::getManageDLL()->getdlls()[(index-1)];
            dll->loacl_inital(index);
            while(true){

            }
        }
        else
        {
            printf("process id %d\n", pid);
            boost::shared_ptr<Process> process(new Process(pid));
            precesses.push_back(process);
        }
    }
    return true;
}

bool ProcessManage::SetAffinity(int core)
{
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(core, &mask);
    if (sched_setaffinity(0, sizeof(mask), &mask) < 0) {
        perror("sched_setaffinity");
    }
    return true;
}
#endif /* DPDK_FRAMEWORK_PROCESSMANAGE_HPP_ */
