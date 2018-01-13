#include <stdio.h>
#include <iostream>
#include <assert.h>
#include <event/Event.hpp>
#include <framework/ManageRtePool.hpp>
extern "C"
{
void local_initial(unsigned lcore_id)
{

	if(3 != lcore_id)
	{
		return;
	}
	printf("test 2 %d !!!!!\n",lcore_id);
	std::cout<<"start 2 "<<std::endl;

	struct rte_ring *send1 = rte_ring_lookup("message_ring1");
	struct rte_ring *send2 = rte_ring_lookup("message_ring2");
	//struct rte_ring *recv = rte_ring_lookup("message_ring1");

	Event event("message_ring3", "MEM_POOL", false);

	event.setTargetQueue(send1);
	char a[]={"1111111111111111111"};
	event.send(a);

	event.setTargetQueue(send2);
	char b[]={"22222222222222222222"};
	event.send(b);
}
void global_initial()
{
struct rte_ring *ring_id;
ring_id = rte_ring_create("message_ring3",
	        64, rte_socket_id(), 0);
if(NULL == ring_id)
{
	assert( false );
}
}
}
