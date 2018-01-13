/*
 * ManageDLL.hpp
 *
 *  Created on: Jan 2, 2018
 *      Author: lifuxin
 */

#ifndef DPDK_FRAMEWORK_MANAGEDLL_HPP_
#define DPDK_FRAMEWORK_MANAGEDLL_HPP_
#include <string>
#include <stdio.h>
#include <dlfcn.h>
#include <assert.h>
#include<boost/smart_ptr.hpp>
#include <vector>
class DllInfo
{
public:
	DllInfo(std::string name);
	~DllInfo();
	void (*gloal_inital)();
	void (*loacl_inital)(unsigned lcore_id);
private:
	std::string name;
	void *handle;
};

DllInfo::DllInfo(std::string name):name(name)
{
	void* error;
	handle = dlopen(name.c_str(), RTLD_NOW);
	if(!handle)
	{
		printf("open so fail\n");
		assert(0);
	}
	dlerror();
	gloal_inital=(void (*)())dlsym(handle,"global_initial");
	if ((error = dlerror()) != NULL)
	{
	  fprintf(stderr, "%s\n", error);
	  assert(0);
	}

    dlerror();
	loacl_inital=(void (*)(unsigned lcore_id))dlsym(handle,"local_initial");
	if ((error = dlerror()) != NULL)
	{
	    fprintf(stderr, "%s\n", error);
	    assert(0);
	}

}

DllInfo::~DllInfo()
{
	dlclose(handle);
}
class ManageDLL
{
public:
	static ManageDLL* getManageDLL();
	std::vector<boost::shared_ptr<DllInfo> >& getdlls();
private:

	std::vector<boost::shared_ptr<DllInfo> > dlls;
	static ManageDLL* instance;
};

ManageDLL* ManageDLL::getManageDLL()
{
	return instance;
}

std::vector<boost::shared_ptr<DllInfo> >& ManageDLL::getdlls()
{
	return dlls;
}

ManageDLL* ManageDLL::instance = new ManageDLL();
#endif /* DPDK_FRAMEWORK_MANAGEDLL_HPP_ */
