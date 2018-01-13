/*
 * main.cpp
 *
 *  Created on: Dec 5, 2017
 *      Author: lifuxin
 */

#include <rte_cryptodev.h>
#include <rte_eal.h>
#include <rte_ethdev.h>
#include <rte_cycles.h>
#include <rte_lcore.h>

#include <rte_malloc.h>
#include <rte_mbuf.h>
#include <rte_memcpy.h>
#include <rte_memory.h>
#include <rte_mempool.h>
#include <event/Event.hpp>
#include <assert.h>
#include <iostream>

#define RX_RING_SIZE 128        //接收环大小
#define TX_RING_SIZE 512        //发送环大小

#define NUM_MBUFS 8292
#define MBUF_CACHE_SIZE 256
#define BURST_SIZE 32
#define PTP_PROTOCOL             0x88F7

struct rte_mempool *mbuf_pool;
struct rte_ring *ring1;
struct rte_ring *ring2;


void start1()
{
	sleep(1);
	std::cout<<"start1 "<<std::endl;

	struct rte_ring *send = rte_ring_lookup("message_ring2");
	//struct rte_ring *recv = rte_ring_lookup("message_ring2");

	Event event("message_ring1", "MEM_POOL", false);
	event.setTargetQueue(send);
	char a[]={"1111111111111111111"};
	event.send(a);
}

void start2()
{
	std::cout<<"start2 "<<std::endl;

	//struct rte_ring *send = rte_ring_lookup("message_ring2");
	//struct rte_ring *recv = rte_ring_lookup("message_ring1");

	Event event("message_ring2", "MEM_POOL", false);

	sleep(2);
	char a[]={"1111111111111111111"};
	void* data;
	std::cout<<"start receive "<<std::endl;
	data = event.recv();
	std::cout<<"recv : "<<(char* )data<<std::endl;
}

static int start(__attribute__((unused)) void *arg)
{
	unsigned lcore_id;
	lcore_id = rte_lcore_id();
	std::cout<<"=====[]lcore_id : "<<lcore_id<<std::endl;

	switch (lcore_id)
	{
		case 1:
			start1();
		break;
		case 2:
			start2();
		break;
	}
	return 0;
}

int main(int argc, char **argv)
{
	const unsigned flags = 0;
	int ret;
	unsigned lcore_id;
	unsigned nb_ports;
	const unsigned ring_size = 64;
	ret = rte_eal_init(argc, argv);
	std::cout<<"===========  ret : "<<ret<<std::endl;
	nb_ports = rte_eth_dev_count();
	std::cout<<"===============nb_ports : "<<nb_ports<<std::endl;


	mbuf_pool = rte_pktmbuf_pool_create("MEM_POOL",NUM_MBUFS * rte_eth_dev_count(),
	MBUF_CACHE_SIZE,0,RTE_MBUF_DEFAULT_BUF_SIZE,rte_socket_id());

	rte_ring_create("message_ring1",
	            64, rte_socket_id(), 0);
	rte_ring_create("message_ring2",
	            64, rte_socket_id(), 0);
	//ring1 = rte_ring_create("message_ring1",
	//        ring_size, rte_socket_id(), flags);

	//ring2 = rte_ring_create("message_ring2",
	//	        ring_size, rte_socket_id(), flags);

	RTE_LCORE_FOREACH_SLAVE(lcore_id) {
				rte_eal_remote_launch(start, NULL, lcore_id);
			}
	sleep(3);

	return 0;
};
