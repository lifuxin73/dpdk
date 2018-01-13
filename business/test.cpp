/*
 * main.cpp
 *
 *  Created on: Jan 1, 2018
 *      Author: lifuxin
 */

#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

#include <stdio.h>
#include <iostream>
#include <assert.h>
#include <event/Event.hpp>
#include <framework/ManageRtePool.hpp>

#define FLAGS O_WRONLY | O_CREAT | O_TRUNC
#define MODE S_IRWXU | S_IXGRP | S_IROTH | S_IXOTH
extern "C"
{
void local_initial(unsigned lcore_id)
{

    if(1 != lcore_id)
    {
        return;
    }
    printf("test 0%d !!!!!\n",lcore_id);
    std::cout<<"start 0 "<<std::endl;

    Event event("message_ring1", "MEM_POOL", false);
    sleep(1);
    void* data;
    std::cout<<"start receive "<<std::endl;
    data = event.recv();
    std::cout<<"recv : "<<(char* )data<<std::endl;
}

void global_initial()
{
	int fd;//文件描述符
	  if ((fd = open("test11111111111111111", FLAGS, MODE)) == -1) {
	    printf("open file error");
	    return ;
	  }

    struct rte_ring *ring_id;
    ring_id = rte_ring_create("message_ring1",
            64, rte_socket_id(), 0);
    if(NULL == ring_id)
    {
        assert( false );
    }
}
}
