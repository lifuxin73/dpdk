/*
 * Event.hpp
 *
 *  Created on: Dec 5, 2017
 *      Author: lifuxin
 */

#ifndef DPDK_EVENT_EVENT_HPP_
#define DPDK_EVENT_EVENT_HPP_
#include<cstring>
#include<stdlib.h>
#include<unistd.h>
#include <assert.h>
#include <rte_mempool.h>
#include <rte_ring.h>

#include <iostream>
class Event
{
public:
    Event(std::string queueName, std::string rte_mempool_name, bool isCreateRing);
    void setTargetQueue(struct rte_ring *send_ring);
    int send(void* data);
    void* receive();
    void* recv();
private:
    struct rte_ring *send_ring;
    struct rte_ring *recv_ring;
    std::string queueName;
    struct rte_mempool *mbuf_pool;
};


void Event::setTargetQueue(struct rte_ring *send_ring)
{
	this->send_ring = send_ring;
}


Event::Event(std::string queueName, std::string rte_mempool_name,bool isCreateRing = true):
    send_ring(NULL),
    queueName(queueName)
{
    if(isCreateRing)
    {
        recv_ring = rte_ring_create(queueName.c_str(),
            64, rte_socket_id(), 0);
    }
    else
    {
        recv_ring = rte_ring_lookup(queueName.c_str());
        if(NULL == recv_ring)
        {
            assert(false);
        }
    }

    mbuf_pool = rte_mempool_lookup(rte_mempool_name.c_str());
}

int Event::send(void* data)
{
    void* msg;
    std::cout<<"++++++ send"<<std::endl;
    if (rte_mempool_get(mbuf_pool, &msg) < 0)
    {
        std::cout<<"rte_mempool_get fail"<<std::endl;
        return -1;
    }

    memcpy(msg, data, sizeof(data));
    if (rte_ring_enqueue(send_ring, msg) < 0) {
        rte_mempool_put(mbuf_pool, msg);
        std::cout<<"rte_ring_enqueue fail"<<std::endl;
        return -1;
    }

    return 0;

}

void* Event::recv()
{
    void* data;
    void* msg;
    while(true)
    {
        if (rte_ring_dequeue(recv_ring, &msg) < 0)
        {
            usleep(5);
            continue;
        }
        break;
    }

    data = malloc(sizeof(msg));
    memcpy(data, msg, sizeof(msg));

    rte_mempool_put(mbuf_pool, msg);

    return data;
}

#endif /* DPDK_EVENT_EVENT_HPP_ */
