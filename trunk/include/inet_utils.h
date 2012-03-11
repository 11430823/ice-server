/**
 *============================================================
 *  @file      inet_utils.h
 *  @brief    
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */
#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 用于通过网卡接口（eth0/eth1/lo...）获取对应的IP地址。支持IPv4和IPv6。
 * @param nif 网卡接口。eth0/eth1/lo...
 * @param af 网络地址类型。AF_INET或者AF_INET6。
 * @param ipaddr 用于返回nif和af对应的IP地址。ipaddr的空间由函数调用者分配，并且长度必须大于或者等于IP地址的长度（16或者46字节）。
 * @param len ipaddr的长度（字节）。
 * @return 成功返回0，并且ipaddr中保存了nif和af对应的IP地址。失败返回-1。
 */
int get_ip_addr(const char* nif, int af, void* ipaddr, size_t len);



#ifdef __cplusplus
}
#endif

