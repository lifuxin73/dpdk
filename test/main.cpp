#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <sys/queue.h>

extern "C"
{
#include <rte_memory.h>
#include <rte_launch.h>
#include <rte_eal.h>
#include <rte_per_lcore.h>
#include <rte_lcore.h>
#include <rte_debug.h>
#include <rte_ethdev.h>
}
#include <iostream>

int
main(int argc, char **argv)
{
	int ret;
	unsigned lcore_id;
	unsigned nb_ports;

	ret = rte_eal_init(argc, argv);
	std::cout<<"===========  ret : "<<ret<<std::endl;
	nb_ports = rte_eth_dev_count();
	std::cout<<"===============nb_ports : "<<nb_ports<<std::endl;
	return 0;
}
