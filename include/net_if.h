/**
 *============================================================
 *  @file        net_if.h
 *  @brief      Essential net interface to deal with network
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef ASYNC_SERVER_NET_INTERFACE_H_
#define ASYNC_SERVER_NET_INTERFACE_H_

#include <stdint.h>
#include <netinet/in.h>

#include "service.h"
#include "net.h"

/**
  * @brief hold an ip and a port
  */
typedef struct ip_port {
	/*! ip */
	char		ip[16];
	/*! port */
	in_addr_t	port;
} ip_port_t;

/**
  * @brief Connect to a given server.
  *
  * @param const char* ipaddr,  ip address of the server to connect to.
  * @param in_addr_t port,  port of the server to connect to.
  * @param int bufsz,  size of the buffer to hold the sending data.
  * @param int timeout, interrupt the connecting attempt after timeout secs.
  *
  * @return int, the connected socket fd, -1 on error.
  */
int connect_to_svr(const char* ipaddr, in_addr_t port, int bufsz, int timeout);

/**
  * @brief Connect to a given server whose name is 'service_name'.
  *
  * @param service_name name of the server to connect to.
  * @param svr_id id of the 'service_name' to connect to. If svr_id is assigned with 0, then a random server id is generated.
  * @param int bufsz,  size of the buffer to hold the sending data.
  * @param int timeout, interrupt the connecting attempt after timeout secs.
  *
  * @return int, the connected socket fd, -1 on error.
  */
int connect_to_service(const char* service_name, uint32_t svr_id, int bufsz, int timeout);

/**
  * @brief �첽���ӵ�ָ��������
  *
  * @param ipaddr ������IP
  * @param port �������˿�
  * @param bufsz ���ͻ�������С���ֽڣ�
  * @param callback �ص��������ûص������ĵ�һ������fd��AsyncServer��ܱ����ݸ��û��ṩ��callback������fd������-1���ʾ���ӳɹ���
  *                 �ڶ�������arg���û��ṩ��ά�������ڱ���һЩ��ʶ��Ϣ����callback������ʱ�����Ը���arg���������ִ����ȷ���߼���\n
  *                 ע�⣺��fd������-1�����Ǹ���arg���ָ�fd�����Ѿ��������߼����û���Ҫ����close_svr���رո�fd��
  * @param arg ���û��ṩ��ά�������ڱ���һЩ��ʶ��Ϣ����callback������ʱ�����Ը���arg���������ִ����ȷ���߼�
  *
  * @return �ɹ�����0��ʧ�ܷ���-1��\n
  *         ע�⣺����-1�Ļ����������callback���û���ʱ��Ҫ�ͷ�arg������arg�Ƕ�̬����Ļ�����
  *         ����0�Ļ��������������ӽ����ɹ���񣬶������callback���û�������callback���ͷ�arg������arg�Ƕ�̬����Ļ�����\n
  *         �������ڲ��������callback��
  */
int asyn_connect_to_svr(const char* ipaddr, in_addr_t port, int bufsz, void (*callback)(int fd, void* arg), void* arg);

/**
  * @brief �첽���ӵ�����Ϊ'service_name'�ķ�����
  *
  * @param service_name ����������
  * @param svr_id 'service_name'��������ID�����svr_idΪ0�����������һ�����õķ�����
  * @param bufsz ���ͻ�������С���ֽڣ�
  * @param callback �ص��������ûص������ĵ�һ������fd��AsyncServer��ܱ����ݸ��û��ṩ��callback������fd������-1���ʾ���ӳɹ���
  *                 �ڶ�������arg���û��ṩ��ά�������ڱ���һЩ��ʶ��Ϣ����callback������ʱ�����Ը���arg���������ִ����ȷ���߼���\n
  *                 ע�⣺��fd������-1�����Ǹ���arg���ָ�fd�����Ѿ��������߼����û���Ҫ����close_svr���رո�fd��
  * @param arg ���û��ṩ��ά�������ڱ���һЩ��ʶ��Ϣ����callback������ʱ�����Ը���arg���������ִ����ȷ���߼�
  *
  * @return �ɹ�����0��ʧ�ܷ���-1\n
  *         ע�⣺����-1�Ļ����������callback���û���ʱ��Ҫ�ͷ�arg������arg�Ƕ�̬����Ļ�����
  *         ����0�Ļ��������������ӽ����ɹ���񣬶������callback���û�������callback���ͷ�arg������arg�Ƕ�̬����Ļ���\n
  *         �������ڲ��������callback��
  */
int asyn_connect_to_service(const char* service_name, uint32_t svr_id, int bufsz, void (*callback)(int fd, void* arg), void* arg);

/**
  * @brief �����رպͷ���ˣ�Switch��DBproxy�ȣ������ӡ��������on_fd_closed��
  * @param svrfd connect_to_svr/connect_to_service���ص�fd
  * @see connect_to_svr, connect_to_service
  */
void close_svr(int svrfd);

/**
  * @brief Create a udp socket to the given server.
  *
  * @param struct sockaddr_in* addr,  it will be initialized base on the given ip and port.
  * @param const char* ip,  ip address of the server.
  * @param in_port_t port,  port of the server.
  *
  * @return int, the created udp socket fd, -1 on error.
  */
int create_udp_socket(struct sockaddr_in* addr, const char* ip, in_port_t port);

/**
  * @brief resolve ip of a given service name and service id
  *
  * @param service_name name of the server ip to resolve
  * @param svr_id id of the 'service_name' to resolve
  *
  * @return the resolved ip or 0
  */
const char* resolve_service_name(const char* service_name, uint32_t svr_id);

/**
  * @brief get ip and port of the service that was most lately try connecting to
  *
  * @return the pointer to ip_port_t or 0
  */
const ip_port_t* get_last_connecting_service();

/**
  * @brief get ip address of a remote server connection
  * @param fd fd of connection to the remote server
  * @return ip address of a client
  */
static inline
uint32_t get_remote_ip(int fd)
{
	if ((fd >= 0) && (fd <= epi.maxfd) && (epi.fds[fd].type != fd_type_unused)) {
		return epi.fds[fd].sk.remote_ip;
	}

	return 0;
}

/**
  * @brief Send data to the given tcp socket fd
  *
  * @param int fd,  socket fd to send data to.
  * @param const void* data,  data to be sent to the given fd.
  * @param uint32_t len,  length of the data to be sent.
  *
  * @return int, 0 on sucess, -1 on failure.
  */
int net_send(int fd, const void* data, uint32_t len);

/**
  * @brief Send a package to a client. For child process only.
  *
  * @param fdsess  fd session of the client to whom the package will be sent.
  * @param pkg  package to be sent.
  * @param pkglen  length of the package to be sent.
  *
  * @return 0 on success, -1 on failure
  */
int send_pkg_to_client(fdsession_t* fdsess, const void* pkg, const int pkglen);

/**
  * @brief Close a client connection. For child process only. \n
  *           This function will firstly call on_client_conn_closed implemented by the .so lib,
  *           then it'll send a connection-close-request to the parent process.
  *
  * @param fd client socket fd to be closed
  */
void close_client_conn(int fd);
#endif // ASYNC_SERVER_NET_INTERFACE_H_

