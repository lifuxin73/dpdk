/*
 * ManageRtePool.hpp
 *
 *  Created on: Jan 7, 2018
 *      Author: lifuxin
 */

#ifndef DPDK_FRAMEWORK_MANAGERTEPOOL_HPP_
#define DPDK_FRAMEWORK_MANAGERTEPOOL_HPP_

#include <rte_mempool.h>
#include <map>
#include<iostream>

class ManageRtePool
{
public:
	static ManageRtePool* getManageRtePool();
	std::map<int,struct rte_mempool*>& getMempool();
private:
	static ManageRtePool* instance;
	std::map<int,struct rte_mempool*> mempools;
};

ManageRtePool* ManageRtePool::instance =new  ManageRtePool();
static std::map<int,struct rte_mempool*> mempools;

std::map<int,struct rte_mempool*>& ManageRtePool::getMempool()
{
	return mempools;
}
ManageRtePool* ManageRtePool::getManageRtePool()
{
	return instance;
}


#endif /* DPDK_FRAMEWORK_MANAGERTEPOOL_HPP_ */
