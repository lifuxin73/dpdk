/*
 * main.cpp
 *
 *  Created on: Jan 1, 2018
 *      Author: lifuxin
 */
#include <stdio.h>
#include <dlfcn.h>
#include <assert.h>
#include <sys/time.h>
#include <rte_ethdev.h>
#include <rte_lcore.h>

#include <event/Event.hpp>
#include "ManageDLL.hpp"
#include "ProcessManage.hpp"
#include "ManageRtePool.hpp"

#define NUM_MBUFS 8292
#define MBUF_CACHE_SIZE 256

int main(int argc, char **argv)
{
    int ret;
    ret = rte_eal_init(argc, argv);

    boost::shared_ptr<DllInfo> d(new DllInfo("../business/libtest.so"));
    boost::shared_ptr<DllInfo> d1(new DllInfo("../business/libtest1.so"));
    boost::shared_ptr<DllInfo> d2(new DllInfo("../business/libtest2.so"));
    ManageDLL* manageDLL = ManageDLL::getManageDLL();
    manageDLL->getdlls().push_back(d);
    manageDLL->getdlls().push_back(d1);
    manageDLL->getdlls().push_back(d2);

    for(boost::shared_ptr<DllInfo> dll: manageDLL->getdlls())
    {
        dll->gloal_inital();
    }
    std::map<int,struct rte_mempool*> mempools = ManageRtePool::getManageRtePool()->getMempool();
    struct rte_mempool* m = rte_pktmbuf_pool_create("MEM_POOL",NUM_MBUFS * rte_eth_dev_count(),
            MBUF_CACHE_SIZE,0,RTE_MBUF_DEFAULT_BUF_SIZE,rte_socket_id());
    if(m==NULL)
    {
        printf("aaaaaaaaaaaaaaaaaaa");
    }
    mempools.insert(std::pair<int,rte_mempool*>(0, m));
	 //for (boost::shared_ptr<DllInfo> dll : manageDLL->getdlls())
	 //{
	//	 dll->fn1(1000);
	 //}
    ProcessManage processManage(4);
    processManage.CreateChilden(manageDLL);
    printf("childen process %d\n",processManage.getPrecesses().size());


    sleep(3);
    processManage.waitProcess();



    return 0;
}
