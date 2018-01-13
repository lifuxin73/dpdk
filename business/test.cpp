/*
 * main.cpp
 *
 *  Created on: Jan 1, 2018
 *      Author: lifuxin
 */
#include <stdio.h>
#include <iostream>
#include <assert.h>
#include <event/Event.hpp>
#include <framework/ManageRtePool.hpp>
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
    struct rte_ring *ring_id;
    ring_id = rte_ring_create("message_ring1",
            64, rte_socket_id(), 0);
    if(NULL == ring_id)
    {
        assert( false );
    }
}
}
