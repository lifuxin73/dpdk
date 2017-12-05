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
#include <rte_mempool.h>
#include <rte_ring.h>

#include <iostream>
class Event
{
public:
	Event(struct rte_ring *send_ring, struct rte_ring *recv_ring, struct rte_mempool *mbuf_pool);
	int send(void* data);
	void* receive();
	void* recv();
private:
	struct rte_ring *send_ring;
	struct rte_ring *recv_ring;
	struct rte_mempool *mbuf_pool;
};

Event::Event(struct rte_ring *send_ring, struct rte_ring *recv_ring, struct rte_mempool *mbuf_pool):
		send_ring(send_ring),
		recv_ring(recv_ring),
		mbuf_pool(mbuf_pool)
{

}

int Event::send(void* data)
{
	void* msg;
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
