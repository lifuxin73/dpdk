#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <sys/queue.h>
#include<netinet/in.h>
#include<cstring>
#include <unistd.h>

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

#include <message/student.hpp>
#include <iostream>

#define RX_RING_SIZE 128        //接收环大小
#define TX_RING_SIZE 512        //发送环大小

#define NUM_MBUFS 8292
#define MBUF_CACHE_SIZE 256
#define BURST_SIZE 32
#define PTP_PROTOCOL             0x88F7

//static const struct rte_eth_conf port_conf_default ;

struct rte_mempool *mbuf_pool;

static inline int
port_init(uint8_t port, struct rte_mempool *mbuf_pool)
{
	rte_eth_conf port_conf_default1 ;
	port_conf_default1.rxmode.mq_mode = ETH_MQ_RX_NONE;
	port_conf_default1.rxmode.max_rx_pkt_len = ETHER_MAX_LEN;
	port_conf_default1.rxmode.split_hdr_size = 0;
	port_conf_default1.rxmode.header_split = 0;
	port_conf_default1.rxmode.hw_ip_checksum = 0;
	port_conf_default1.rxmode.hw_vlan_filter = 0;
	port_conf_default1.rxmode.jumbo_frame = 0;
	port_conf_default1.rxmode.hw_strip_crc = 0;

	port_conf_default1.txmode.mq_mode = ETH_MQ_TX_NONE;

	port_conf_default1.link_speeds = 0;
	//rte_eth_conf port_conf_default1 ;
	//port_conf_default1.rxmode.max_rx_pkt_len = ETHER_MAX_LEN;

	struct rte_eth_conf port_conf = port_conf_default1;

	const uint16_t rx_rings = 1, tx_rings = 1;
	int retval;
	uint16_t q;
	struct rte_eth_link link;
	if(port >= rte_eth_dev_count())
	{
		return -1;
	}

	retval = rte_eth_dev_configure(port, rx_rings, tx_rings, &port_conf);

	if(retval !=0)
	{	std::cout<<"===============rte_eth_dev_configure: "<<retval<<std::endl;
		return retval;
	}

	uint16_t nb_rxd = RX_RING_SIZE;
	uint16_t nb_txd = TX_RING_SIZE;
	retval = rte_eth_dev_adjust_nb_rx_tx_desc(port, &nb_rxd,
			&nb_txd);
	if (retval < 0) {
		printf("Cannot adjust number of descriptors: err=%d, port=%u\n",
			retval, port);
		return -1;
	}

	for(q = 0; q <rx_rings; q++)
	{
		retval = rte_eth_rx_queue_setup(port, q, RX_RING_SIZE,
				rte_eth_dev_socket_id(port), NULL ,mbuf_pool);

		if(retval < 0)
		{
			return retval;
		}
	}

	 for (q = 0; q < tx_rings; q++) {
		 retval = rte_eth_tx_queue_setup(port, q, TX_RING_SIZE,
				 rte_eth_dev_socket_id(port), NULL);

		 if(retval < 0)
		 {
			 return retval;
		 }
	 }

	 retval = rte_eth_dev_start(port);

	 if(retval < 0)
	 {
		 return retval;
	 }

	 struct ether_addr addr;

	 rte_eth_macaddr_get(port, &addr);

     printf("Port %u MAC: %02" PRIx8 " %02" PRIx8 " %02" PRIx8
                        " %02" PRIx8 " %02" PRIx8 " %02" PRIx8 "\n",
                     (unsigned)port,
                     addr.addr_bytes[0], addr.addr_bytes[1],
                     addr.addr_bytes[2], addr.addr_bytes[3],
                     addr.addr_bytes[4], addr.addr_bytes[5]);

	 rte_eth_promiscuous_enable(port);

	 std::cout<<"===============rte_eth_promiscuous_enable end : "<<std::endl;
	 /* 查看端口状态 */
		rte_eth_link_get(port, &link);
		if (link.link_status)
	    {
			printf("Port %d Link Up - speed %u Mbps - %s\n",port,
			       (uint32_t) link.link_speed,
			       (link.link_duplex == ETH_LINK_FULL_DUPLEX) ?
			       ("full-duplex") : ("half-duplex\n"));
		}
		else
	    {
			printf("Port %d Link Down\n",port);
		}
	return 0;
}

static void lcore_main()
{
	const uint8_t port_count = rte_eth_dev_count();
	int port = 0;
	for(int p = 0; p < port_count; p++ )
	{
		printf("rte_eth_dev_socket_id :%d\n", rte_eth_dev_socket_id(p));
		printf("rte_socket_id :%d\n", (int)rte_socket_id());

	}

	printf("lcore : %d\n", rte_lcore_id());


	struct message { char data[50];};
	char msg[]={"aaaaaaaaaaaaaaaaaaaaaaa\0"};
	struct ether_hdr *eth_hdr;
	int pkt_size;

    struct rte_mbuf *pkt;
    struct rte_mbuf *pkt1 = rte_pktmbuf_alloc(mbuf_pool);
    struct rte_mbuf *bufs2[10];
    for(int i=0;i<10;i++)
    {
    	bufs2[i]= new rte_mbuf();
    }
    for(int i=0; i<10 ;i++)
    {

    			pkt = rte_pktmbuf_alloc(mbuf_pool);
    	        pkt_size = sizeof(struct message) + sizeof(struct ether_hdr);
    	        pkt->data_len = pkt_size;
    	        pkt->pkt_len = pkt_size;
    	        printf("send data_len : %d\n", pkt_size);
    	        eth_hdr = rte_pktmbuf_mtod(pkt, struct ether_hdr *);
    	        //rte_eth_macaddr_get(1, &eth_hdr->d_addr);
    	        //rte_eth_macaddr_get(0, &eth_hdr->s_addr);
    	        //eth_hdr->ether_type = htons(PTP_PROTOCOL);
    	        void* data;

    	        //data = rte_pktmbuf_append(pkt, sizeof(struct message));
    	        data = rte_pktmbuf_mtod(pkt,struct message*);

    	        Student st(18, "lifuxin");
    	        std::cout<<"send age : "<<st.age<<std::endl;
    	        std::cout<<"send name : "<<st.name<<std::endl;

    	        if (data != NULL)
    	        {
    	        	printf("data !=NULL\n");
    	            rte_memcpy(data, &st, sizeof(st));
    	        }

    	        printf("send data_len 1 : %d\n", pkt->pkt_len );





    	 uint16_t nb_tx = rte_eth_tx_burst(0, 0, &pkt, 1);
    	 printf("nb_tx : %d\n", nb_tx);

    	 //rte_pktmbuf_append(pkt1, sizeof(struct message));
    	 uint16_t nb_rx = rte_eth_rx_burst(1, 0, &pkt1, 1);
    	 printf("nb_rx : %d\n", nb_rx);





    	 //data1 = rte_pktmbuf_append(pkt1, sizeof(struct message));


    	 printf("--%d\n",pkt1->data_len);
    	 printf("--%d\n",pkt1->pkt_len);
    	 Student *ad;
    	 void* da;
    	 da = rte_pktmbuf_mtod(pkt1, struct message*);
    		//	 (char*) m->buf_addr + RTE_MIN(RTE_PKTMBUF_HEADROOM, m->buf_len);
    	 ad = (Student *)da;

    	 std::cout<<"receive age : "<<ad->age<<std::endl;
    	 std::cout<<"receive name : "<<ad->name<<std::endl;

    	 //std::cout<<"===========  data : "<<ad[0]<<std::endl;
    	 printf("===================================\n");
    }

}


static int start1()
{
	struct rte_mbuf *pkt;
	struct ether_hdr *eth_hdr;
	int pkt_size;
	struct message { char data[50];};

	std::cout<<"start1"<<std::endl;
	std::cout<<"pid "<<pthread_self()<<std::endl;

	for(int i = 0; i <10 ; i++)
	{
		pkt = rte_pktmbuf_alloc(mbuf_pool);
		pkt_size = sizeof(struct message) + sizeof(struct ether_hdr);
		pkt->data_len = pkt_size;
		pkt->pkt_len = pkt_size;
		printf("send data_len : %d\n", pkt_size);
		eth_hdr = rte_pktmbuf_mtod(pkt, struct ether_hdr *);
		//rte_eth_macaddr_get(1, &eth_hdr->d_addr);
		//rte_eth_macaddr_get(0, &eth_hdr->s_addr);
		//eth_hdr->ether_type = htons(PTP_PROTOCOL);
		void* data;

		//data = rte_pktmbuf_append(pkt, sizeof(struct message));
		data = rte_pktmbuf_mtod(pkt,struct message*);

		Student st(18, "lifuxin");
		std::cout<<"send age : "<<st.age<<std::endl;
		std::cout<<"send name : "<<st.name<<std::endl;

		if (data != NULL)
		{
			printf("data !=NULL\n");
			rte_memcpy(data, &st, sizeof(st));
		}

		printf("send data_len 1 : %d\n", pkt->pkt_len );





		uint16_t nb_tx = rte_eth_tx_burst(0, 0, &pkt, 1);
		printf("nb_tx : %d\n", nb_tx);

	}

	return 0;
}

static int start2()
{
	struct rte_mbuf *pkt1 = rte_pktmbuf_alloc(mbuf_pool);
	std::cout<<"start2"<<std::endl;
	for(int i = 0; i <10 ; i++)
	{
		Student *ad;
		void* da;
		uint16_t nb_rx = rte_eth_rx_burst(1, 0, &pkt1, 1);
		printf("nb_rx : %d\n", nb_rx);
		da = rte_pktmbuf_mtod(pkt1, struct message*);
		//	 (char*) m->buf_addr + RTE_MIN(RTE_PKTMBUF_HEADROOM, m->buf_len);
		ad = (Student *)da;

		std::cout<<"receive age : "<<ad->age<<std::endl;
		std::cout<<"receive name : "<<ad->name<<std::endl;
	}

	return 0;
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


	mbuf_pool = rte_pktmbuf_pool_create("MEM_POOL",NUM_MBUFS * rte_eth_dev_count(),
		MBUF_CACHE_SIZE,0,RTE_MBUF_DEFAULT_BUF_SIZE,rte_socket_id());

	if(mbuf_pool == NULL)
	{
		rte_exit(EXIT_FAILURE, "Cannot create mbuf pool\n");
	}

	for(int portid = 0; portid < nb_ports; portid++)
	{
		if(port_init(portid, mbuf_pool)!=0)
		{
			rte_exit(EXIT_FAILURE, "Cannot init port %"PRIu8 "\n",
			                                        portid);
		}
	}

	int c = rte_lcore_count();
	printf("core count :%d\n",c);
	unsigned a=0;
	unsigned aa=1;
	//rte_eal_remote_launch(start1, NULL, a);
	std::cout<<"main pid "<<pthread_self()<<std::endl;
	std::cout<<"main pid "<<pthread_self()<<std::endl;
	/*RTE_LCORE_FOREACH_SLAVE(lcore_id) {
			rte_eal_remote_launch(start, NULL, lcore_id);
		}*/
	RTE_LCORE_FOREACH_SLAVE(lcore_id) {
				rte_eal_remote_launch(start, NULL, lcore_id);
			}


	sleep(3);

	return 0;
}
