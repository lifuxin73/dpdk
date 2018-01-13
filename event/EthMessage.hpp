/*
 * EthMessage.hpp
 *
 *  Created on: Dec 11, 2017
 *      Author: lifuxin
 */

#ifndef DPDK_EVENT_ETHMESSAGE_HPP_
#define DPDK_EVENT_ETHMESSAGE_HPP_

#include<string.h>

#include <rte_ethdev.h>

#define RX_RING_SIZE 128        //接收环大小
#define TX_RING_SIZE 512        //发送环大小
#define PTP_PROTOCOL             0x88F7
template<typename message>
class EthEvent
{
public:
	EthEvent(int port,struct rte_mempool *mbuf_pool,int rx_rings, int tx_rings);
	uint16_t send(uint64_t mac,message* d);
	uint16_t receive(message** d);
	uint16_t register_callback();
	int initial();
private:
	struct rte_mbuf *pkt;
	struct ether_hdr *eth_hdr;
	struct rte_mempool *mbuf_pool;
	int port;
	int rx_rings;
	int tx_rings;
	struct ether_addr addr;
};

template<typename message>
uint16_t EthEvent<message>::receive(message** d)
{
	void* data;
	struct rte_mbuf *pkt;
	uint16_t nb_rx = rte_eth_rx_burst(port, 0, &pkt, 1);
	printf("nb_rx : %d\n", nb_rx);
	data = rte_pktmbuf_mtod(pkt, struct ether_hdr*);
	data = (data + sizeof(ether_hdr));
	*d = (message*)data;
	return nb_rx;
}

template<typename message>
uint16_t EthEvent<message>::send(uint64_t mac,message* d)
{

	void *data = NULL;
	int pkt_size;
	pkt = rte_pktmbuf_alloc(mbuf_pool);
	pkt_size = 1024 + sizeof(struct ether_hdr);
	pkt->data_len = pkt_size;
	pkt->pkt_len = pkt_size;
	printf("send data_len : %d\n", pkt_size);
	eth_hdr = rte_pktmbuf_mtod(pkt, struct ether_hdr *);

	void *tmp = &(eth_hdr->d_addr.addr_bytes[0]);
	*((uint64_t *)tmp) = ((mac&0x0000000000ff)<<40) | ((mac&0x00000000ff00)<<24) | ((mac&0x000000ff0000)<<8)
			| ((mac&0x0000ff000000)>>8) | ((mac&0x00ff00000000)>>24) | ((mac&0xff0000000000)>>40);
	rte_eth_macaddr_get(port, &eth_hdr->s_addr);
	//rte_eth_macaddr_get(1, &eth_hdr->d_addr);

	eth_hdr->ether_type = htons(PTP_PROTOCOL);
	data = (void*)eth_hdr + sizeof(struct ether_hdr);
	memcpy(data, d, sizeof(message));
	uint16_t nb_tx = rte_eth_tx_burst(0, 0, &pkt, 1);
	return nb_tx;
};

template<typename message>
EthEvent<message>::EthEvent(int port,struct rte_mempool *mbuf_pool,int rx_rings=1, int tx_rings=1):
		mbuf_pool(mbuf_pool),
		port(port),
		rx_rings(rx_rings),
		tx_rings(tx_rings)
{
	pkt = NULL;
	eth_hdr = NULL;

};

template<typename message>
int EthEvent<message>::initial()
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

	rte_eth_dev_configure(port, rx_rings, tx_rings, &port_conf);

	uint16_t nb_rxd = RX_RING_SIZE;
	uint16_t nb_txd = TX_RING_SIZE;

	int retval = rte_eth_dev_adjust_nb_rx_tx_desc(port, &nb_rxd,
				&nb_txd);
	if (retval < 0) {
		printf("Cannot adjust number of descriptors: err=%d, port=%u\n",
			retval, port);
		return -1;
	}

	for(int i = 0;i<rx_rings;i++)
	{
		retval = rte_eth_rx_queue_setup(port, i, RX_RING_SIZE,
						rte_eth_dev_socket_id(port), NULL ,mbuf_pool);

		if(retval < 0)
		{
			printf("rte_eth_rx_queue_setup fail");
			return retval;
		}
	}

	for(int i = 0;i<tx_rings;i++)
	{
		 retval = rte_eth_tx_queue_setup(port, i, TX_RING_SIZE,
						 rte_eth_dev_socket_id(port), NULL);

		 if(retval < 0)
		 {
			 printf("rte_eth_tx_queue_setup fail");
			 return retval;
		 }
	}

	retval = rte_eth_dev_start(port);
	if(retval < 0)
	{
		printf("rte_eth_dev_start fail");
		return retval;
	}

	//rte_eth_promiscuous_enable(port);//网卡设置的是否混咋模式接收信息，如果是混杂模式，说明你任何网络信息都接收，否则只接收你上面设置的mac地址，为目的地址的网络信息
	rte_eth_macaddr_get(port, &addr);
    printf("Port %u MAC: %02" PRIx8 " %02" PRIx8 " %02" PRIx8
                        " %02" PRIx8 " %02" PRIx8 " %02" PRIx8 "\n",
                     (unsigned)port,
                     addr.addr_bytes[0], addr.addr_bytes[1],
                     addr.addr_bytes[2], addr.addr_bytes[3],
                     addr.addr_bytes[4], addr.addr_bytes[5]);

	return 0;
};

#endif /* DPDK_EVENT_ETHMESSAGE_HPP_ */
