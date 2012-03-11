/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		ok
*********************************************************************/

#pragma once

#include <netinet/in.h>
#include <lib_util.h>

namespace ice{
	class lib_tcp_t
	{
		PROPERTY_READONLY_DEFAULT(int, fd);
	public:
		lib_tcp_t();
		//virtual ~lib_tcp_t();
		/**
		* @brief Send `total` bytes of data to `sockfd`
		*
		* @param int sockfd,  socket fd to send data to.
		* @param const void* buf,  data to be sent.
		* @param int total,  number of bytes in total to be sent.
		*
		* @return int, number of bytes sent on success, -1 on error.
		*/
		static int safe_tcp_send_n(int sockfd, const void* buf, int total);
		/**
		* @brief Receive data from `sockfd`
		*
		* @param int sockfd,  socket fd to receive data from.
		* @param const void* buf,  buffer to hold the receiving data.
		* @param int bufsize,  size of `buf`.
		*
		* @return int, number of bytes receive on success, -1 on error, 0 on connection closed by peer.
		*/
		static int safe_tcp_recv(int sockfd, void* buf, int bufsize);
		/**
		* @brief Receive `total` bytes of data from `sockfd`
		*
		* @param int sockfd,  socket fd to receive data from.
		* @param const void* buf,  buffer to hold the receiving data. Size of `buf` should be no less than `total`
		* @param int total,  number of bytes to receive.
		*
		* @return int, number of bytes receive on success, -1 on error, 0 on connection closed by peer.
		*/
		static int safe_tcp_recv_n(int sockfd, void* buf, int total);

		/**
		* @brief Set a timeout on sending data. If you want to disable timeout, just simply
		*           call this function again with millisec set to be 0.
		*
		* @param sockfd socket descriptor to be set.
		* @param millisec timeout in milliseconds.
		*
		* @return 0 on success, -1 on error and errno is set appropriately.
		*/
		static int set_sock_send_timeo(int sockfd, int millisec);
		/**
		* @brief Set a timeout on receiving data. If you want to disable timeout, just simply
		*           call this function again with millisec set to be 0.
		*
		* @param sockfd socket descriptor to be set.
		* @param millisec timeout in milliseconds.
		*
		* @return 0 on success, -1 on error and errno is set appropriately.
		*/
		static int set_sock_rcv_timeo(int sockfd, int millisec);
	protected:
		
	private:
		lib_tcp_t(const lib_tcp_t& cr);
		lib_tcp_t& operator=(const lib_tcp_t& cr);
	};

	class lib_tcp_cli_t : public lib_tcp_t
	{
	public:
		lib_tcp_cli_t(){}
		//virtual ~lib_tcp_cli_t();
		/**
		* @brief Create a TCP connection
		*
		* @param const char* ipaddr,  the ip address to connect to.
		* @param in_port_t port,  the port to connect to.
		* @param int timeout,  abort the connecting attempt after timeout secs. If timeout is less than or equal to 0, 
		*                                then the connecting attempt will not be interrupted until error occurs.
		* @param int block,  true and the connected fd will be set blocking, false and the fd will be set nonblocking.
		*
		* @return int, the connected fd on success, -1 on error.
		*/
		// todo need test connect timeout [3/11/2012 meng]
		static int safe_tcp_connect(const char* ipaddr, in_port_t port, int timeout, bool block);
	protected:
		
	private:
		lib_tcp_cli_t(const lib_tcp_cli_t& cr);
		lib_tcp_cli_t& operator=(const lib_tcp_cli_t& cr);
	};

	class lib_tcp_sever_t : public lib_tcp_t
	{
	public:
		lib_tcp_sever_t(){}
		//virtual ~lib_tcp_sever_t();
		/**
		* @brief Create a listening socket fd
		*
		* @param ipaddr the binding ip address. If this argument is assigned with 0,
		*                                           then INADDR_ANY will be used as the binding address.
		* @param port the binding port.
		* @param type type of socket (SOCK_STREAM or SOCK_DGRAM).
		* @param backlog the maximum length to which the queue of pending connections for sockfd may grow.
		* @param bufsize maximum socket send and receive buffer in bytes, should be less than 10 * 1024 * 1024
		*
		* @return the newly created listening fd on success, -1 on error.
		* @see create_passive_endpoint
		*/
		static int safe_socket_listen(const char* ipaddr, in_port_t port, int type, int backlog, int bufsize);
		/**
		* @brief ����һ��TCP listen socket����UDP socket�����ڽ��տͻ������ݡ�֧��IPv4��IPv6��
		*
		* @param host �����ĵ�ַ��������IP��ַ��Ҳ������������������ݲ���Ϊ��0.0.0.0���������INADDR_ANY��
		* @param serv �����Ķ˿ڡ����������ֶ˿ڣ�Ҳ�����Ƕ˿ڶ�Ӧ�ķ���������dns��time��ftp�ȡ�
		* @param socktype socket�����ͣ�������SOCK_STREAM��TCP������SOCK_DGRAM��UDP����
		* @param backlog δ������Ӷ��еĴ�С��һ����1024���ӡ�
		* @param bufsize socket����/���ͻ����С���ֽڣ�������С��10 * 1024 * 1024��
		*
		* @return �ɹ������´�����fd��ʧ�ܷ���-1��fd����ʱ����ʹ��close�رա�
		* @see safe_socket_listen
		*/
		static int create_passive_endpoint(const char* host, const char* serv, int socktype, int backlog, int bufsize);
		/**
		* @brief Accept a TCP connection
		*
		* @param int sockfd,  the listening fd.
		* @param struct sockaddr_in* peer,  used to return the protocol address of the connected peer process.  
		* @param int block,  true and the accepted fd will be set blocking, false and the fd will be set nonblocking.
		*
		* @return int, the accpected fd on success, -1 on error.
		*/
		static int safe_tcp_accept(int sockfd, struct sockaddr_in* peer, bool block);
	protected:
		
	private:
		lib_tcp_sever_t(const lib_tcp_sever_t& cr);
		lib_tcp_sever_t& operator=(const lib_tcp_sever_t& cr);
	};
	
	
}//end namespace ice
