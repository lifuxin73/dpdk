cmake_minimum_required (VERSION 2.6)

add_compile_options(-std=c++11)

set(CMAKE_CXX_FLAGS "-msse4.2")
#include_directories(/home/lifuxin/dpdk/dpdk/x86_64-native-linuxapp-gcc/include)
include_directories(/home/lifuxin/dpdk/dpdk/x86_64-native-linuxapp-gcc/include )
include_directories(/home/lifuxin/dpdk/use_dpdk_code/dpdk/ )
link_directories(/home/lifuxin/dpdk/dpdk/x86_64-native-linuxapp-gcc/lib /usr/lib64)

SET( CMAKE_BUILD_TYPE Debug )



add_executable(hello main.cpp)

target_link_libraries(hello 
			
		-Wl,--whole-archive	
			rte_ethdev 
			rte_eal
			rte_mempool_ring
			rte_mempool
			rte_pmd_e1000
			rte_pmd_i40e
			rte_pmd_ixgbe
			rte_pmd_virtio
			rte_pmd_vmxnet3_uio
			rte_hash
			rte_kvargs
			rte_bus_pci
			rte_pci
			rte_bus_vdev
			rte_mbuf
			rte_ring
			rte_net
			numa
		 	pthread
			dl			
			-Wl,--no-whole-archive 
)

