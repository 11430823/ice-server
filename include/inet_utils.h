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

#ifndef LIBTAOMEE_INET_UTILS_H_
#define LIBTAOMEE_INET_UTILS_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ����ͨ�������ӿڣ�eth0/eth1/lo...����ȡ��Ӧ��IP��ַ��֧��IPv4��IPv6��
 * @param nif �����ӿڡ�eth0/eth1/lo...
 * @param af �����ַ���͡�AF_INET����AF_INET6��
 * @param ipaddr ���ڷ���nif��af��Ӧ��IP��ַ��ipaddr�Ŀռ��ɺ��������߷��䣬���ҳ��ȱ�����ڻ��ߵ���IP��ַ�ĳ��ȣ�16����46�ֽڣ���
 * @param len ipaddr�ĳ��ȣ��ֽڣ���
 * @return �ɹ�����0������ipaddr�б�����nif��af��Ӧ��IP��ַ��ʧ�ܷ���-1��
 */
int get_ip_addr(const char* nif, int af, void* ipaddr, size_t len);

/**
 * @brief ��getnameinfo��getaddrinfo�Ⱥ������ص�EAI_XXX������ת�������Ƶ�EXXX(errno)�����롣
 * @param eai EAI_XXX������
 * @return �������Ƶ�EXXX(errno)������
 */
int eai_to_errno(int eai);

#ifdef __cplusplus
}
#endif

#endif // LIBTAOMEE_INET_UTILS_H_

