#include <sys/epoll.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>

#include "lib_log.h"
#include "lib_file.h"
#include "lib_time.h"
#include "lib_tcp_server_epoll.h"

#if 0
int KM_TCP_S::__handle_message_epoll(SOCKET s)
{
    char buf[1500 + 1] = {0};
    int len;
    /* 开始处理每个新连接上的数据收发 */
    /* 接收客户端的消息 */
    len = recv(s, buf, sizeof(buf)-1, 0);
    switch(len)
    {
    	case 0:
			{
				m_ilog.log("client close! 0 == recv");
			}
    	break;
    	case SOCKET_ERROR:
    		break;
    	default:
    		{
    			buf[len] = '\0';
    			cout << buf << endl;
					int i = send(s,buf,len,0);
					cout << "send : " << i << endl;
				}
    		break;
    }
    /*ET模式，即，边沿触发，类似于电平触发，epoll中的边沿触发的意思是只对新到的数据进行通知，而内核缓冲区中如果是旧数据则不进行通知，所以在do_use_fd函数中应该使用如下循环，才能将内核缓冲区中的数据读完。
            while (1) {
           len = recv(*******);
           if (len == -1) {
             if(errno == EAGAIN)
                break;
             perror("recv");
             break;
           }
           do something with the recved data........
        }
    */

    return len;
}

#endif

int ice::lib_tcp_server_epoll_t::run( CHECK_RUN check_run_fn )
{
	while(check_run_fn()){
		sleep(1);
	}

	
#if 0
	
	int ret = 0;//返回值
	int eve_num = 0;//事件的数量
	int nCountFDs = 1;//epoll 中的套接字数量
	int i = 0;
	epoll_event evs[this->max_events_num];
	SOCKET accept_s;//连接上的客户SOCKET
	sockaddr_in sa_in_peer;//对方sockaddr_in
	memset(&sa_in_peer,0,sizeof(sa_in_peer));
	socklen_t nSocklen;
	epoll_event ev;
	int nn = 1;
	int nTimeOut = 1000;
	while(m_bRun)
	{
		//cout << nn++ << endl;
		//wait event happen
		eve_num = epoll_wait(this->fd, evs, this->max_events_num, this->epoll_wait_time_out);
		switch(eve_num)
		{
		case 0://time out
			continue;//while
			break;
		case -1:{
				ALERT_LOG("epoll wait err [%s]", ::strerror(errno));
				return -1;
			}
			break;
		default:
			break;
		}

		//handling event
		for (i = 0; i < eve_num; ++i){
			if (m_s == evs[i].data.fd)
			{// 处理来自监听端的连接
				accept_s = accept(m_s, (struct sockaddr *) &sa_in_peer,&nSocklen);
				if (accept_s < 0) 
				{
					perror("accept:");
					continue;
				} 
				else
				{
					cout << accept_s << endl;
					m_ilog.log("client from:%s:%d,socket == %d",inet_ntoa(sa_in_peer.sin_addr),ntohs(sa_in_peer.sin_port),accept_s);
				}

				set_nonblocking(accept_s);
				ev.events = EPOLLIN ;//| EPOLLOUT;//可读 | 可写   //EPOLLIN | EPOLLET;
				ev.data.fd = accept_s;
				if (epoll_ctl(m_nFD, EPOLL_CTL_ADD, accept_s, &ev) < 0)
				{
					m_ilog.err("file:%s,line:%d,add socket:%d in epoll false! %s,",__FILE__,__LINE__,accept_s,strerror(errno));
					return -1;
				}
				nCountFDs++;
			}
			else 
			{//可读或可写(可同时发生,并不互斥)
				if(evs[i].events & EPOLLIN)
				{//接收并处理其他套接字的数据
					ret = __handle_message_epoll(evs[i].data.fd);
					switch(ret)
					{
					case 0:
						{
							int e = epoll_ctl(m_nFD, EPOLL_CTL_DEL, evs[i].data.fd,&ev);
							if(SOCKET_ERROR == e)
							{
								m_ilog.err("file:%s,line:%d,errno:%d,%s",__FILE__,__LINE__,errno,strerror(errno));
							}
							nCountFDs--;
							close_socket(evs[i].data.fd);
						}
						break;
					case SOCKET_ERROR:
						{
							if(11 != errno)
							{// 断开
								m_ilog.err("file:%s,line:%d,errno:%d,%s",__FILE__,__LINE__,errno,strerror(errno));
								perror("11 != errno:");
								int e = epoll_ctl(m_nFD, EPOLL_CTL_DEL, evs[i].data.fd,&ev);
								nCountFDs--;
							}	
						}
						break;
					default:
						break;
					}
				}

				if(evs[i].events & EPOLLOUT)
				{//该套接字可写
					m_ilog.log("file:%s,line:%d,evs[i].events & EPOLLOUT",__FILE__,__LINE__);
				}
			}
		}
	}
#endif
	return 0;
}

ice::lib_tcp_server_epoll_t::lib_tcp_server_epoll_t(uint32_t max_events_num)
{
	this->max_events_num = max_events_num;
	this->epoll_wait_time_out = -1;
	this->on_pipe_event = NULL;
	this->on_functions = NULL;
}

int ice::lib_tcp_server_epoll_t::listen(const char* ip, uint16_t port, uint32_t listen_num)
{
	//Since Linux 2.6.27  SOCK_NONBLOCK | SOCK_CLOEXEC
	this->listen_fd = ::socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
	if (-1 == this->listen_fd){
		ALERT_LOG("create socket err [%s]", ::strerror(errno));
		return -1;
	}

	lib_file_t::set_io_block(this->listen_fd, false);

	if (0 != this->bind(ip, port)){
		lib_file_t::close_fd(this->listen_fd);
		ALERT_LOG("bind socket err [%s]", ::strerror(errno));
		return -1;
	}

	if (0 != ::listen(this->listen_fd, listen_num)){
		ALERT_LOG("listen err [%s]", ::strerror(errno));
		return -1;
	}

	sockaddr_in sa_in;
	memset(&sa_in, 0, sizeof(sa_in));
	sa_in.sin_addr.s_addr = inet_addr(ip);
	sa_in.sin_family = PF_INET;
	sa_in.sin_port = htons(port);
	if (0 != this->add_connect(this->listen_fd, FD_TYPE_LISTEN, &sa_in)){
		ALERT_LOG("add connect err[%s]", ::strerror(errno));
		return -1;
	}

	return 0;
}

int ice::lib_tcp_server_epoll_t::create()
{
	if ((this->fd = epoll_create(this->max_events_num)) < 0) {
		ALERT_LOG("EPOLL_CREATE FAILED [ERROR:%s]", strerror (errno));
		return -1;
	}

	this->cli_fd_infos = (struct cli_fd_info_t*) new cli_fd_info_t[this->max_events_num];
	if (NULL == this->cli_fd_infos){
		lib_file_t::close_fd(this->fd);
		ALERT_LOG ("CALLOC CLI_FD_INFO_T FAILED [MAXEVENTS=%d]", this->max_events_num);
		return -1;
	}
	return 0;
}

int ice::lib_tcp_server_epoll_t::add_events( int fd, uint32_t flag )
{
	epoll_event ev;
	ev.events = flag;
	ev.data.fd = fd;

	int ret = HANDLE_EINTR(::epoll_ctl(this->fd, EPOLL_CTL_ADD, fd, &ev));
	if (0 != ret){
		ERROR_LOG ("epoll_ctl add fd:%d error:%s", fd, strerror(errno));
		return -1;
	}

	return 0; 
}

int ice::lib_tcp_server_epoll_t::add_connect( int fd, E_FD_TYPE fd_type, struct sockaddr_in* peer )
{
	uint32_t flag;

	switch (fd_type) {
	default:
		flag = EPOLLIN;
		break;
	}

	if (0 != this->add_events(fd, flag)) {
		return -1;
	}

	cli_fd_info_t& cfi = this->cli_fd_infos[fd];
	cfi.init();
	cfi.fd = fd;
	cfi.fd_type = fd_type;
	cfi.last_tm = ice::lib_time_t::get_now_second();
	if (NULL != peer) {
		cfi.remote_ip = peer->sin_addr.s_addr;
		cfi.remote_port = peer->sin_port;
	}

	TRACE_LOG("time now:%u, fd:%d, fd type:%d", cfi.last_tm, fd, fd_type);
	return 0;
}

int ice::lib_tcp_server_epoll_t::destroy()
{
	for (uint32_t i = 0; i < this->max_events_num; i++) {
		cli_fd_info_t& cfi = cli_fd_infos[i];
		if (FD_TYPE_UNUSED == cfi.fd_type){
			continue;
		}
		lib_file_t::close_fd(cfi.fd);
	}

	safe_delete_arr(this->cli_fd_infos);

	lib_file_t::close_fd(this->fd);
	return 0;
}

void ice::lib_tcp_server_epoll_t::register_pipe_event_fn( ON_PIPE_EVENT fn )
{
	this->on_pipe_event = fn;
}


int service_run()
{
#if 0
	enum {
		trash_size		= 4096,
		mcast_pkg_size	= 8192,
		udp_pkg_size	= 8192
	};
	int max_len = 0;

		epoll_event evs[this->max_ev_num];
	int nr = 0;
	int pos = 0;
	epoll_event ev;
	while ( !g_daemon.stop || 0 != g_dll.on_fini(g_is_parent) ) {
		nr = epoll_wait(this->fd , evs, this->max_ev_num, EPOLL_TIME_OUT);

		if (unlikely(-1 == nr && errno != EINTR)){
			ALERT_LOG("EPOLL_WAIT FAILED, [maxfd=%d, epfd=%d, error:%s]",
				this->max_fd, this->fd, strerror(errno));
			return -1;
		}

		ice::renew_now();

		for (pos = 0; pos < nr; pos++) {
			int fd = evs[pos].data.fd;
			fdinfo_t& fdinfo = this->m_fds[fd];
			if (fd > this->max_fd || fdinfo.fd != fd || fdinfo.fd_type == fd_type_unused) {
				ERROR_LOG("DELAYED EPOLL EVENTS [event fd=%d, cache fd=%d, maxfd=%d, type=%d]", 
					fd, fdinfo.fd, this->max_fd, fdinfo.fd_type);
				continue;
			}

			if ( unlikely(fd_type_pipe == fdinfo.fd_type ) ) {
				if (0 == handle_pipe_event(fd, evs[pos])) {
					continue;
				} else {
					return -1;
				}
			}

			if (evs[pos].events & EPOLLIN) {
				switch (fdinfo.fd_type) {
				case fd_type_listen:
					//accept
					//////////////////////////////////////////////////////////////////////////
					struct sockaddr_in peer;
					int newfd;

					newfd = ice::lib_tcp_sever_t::safe_tcp_accept(fd, &peer, false);
					if (-1 != newfd) {
						get_server_epoll()->add_connect(newfd, fd_type_remote, &peer);
						//todo 后面的错误处理
					} else if ((errno == EMFILE) || (errno == ENFILE)) {
						//add_to_etin_queue(fd);
					} else if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
						//del_from_etin_queue(fd);
					}else{
						ERROR_LOG("ACCEPT ERROR:%s", strerror(errno));
					}
					break;
				case fd_type_remote:
					{
						char buf[1500 + 1] = {0};
						int len;
						/* 开始处理每个新连接上的数据收发 */
						/* 接收客户端的消息 */
						len = recv(fd, buf, sizeof(buf)-1, 0);
						switch(len)
						{
    						case 0:
								{
									//todo 删除
									int e = epoll_ctl(this->fd, EPOLL_CTL_DEL, fd, &ev);
									if(-1 == e)
									{
										//m_ilog.err("file:%s,line:%d,errno:%d,%s",__FILE__,__LINE__,errno,strerror(errno));
									}
									close(fd);
								}
    						break;
    						case -1:
								if(EAGAIN != errno){// 断开
									//m_ilog.err("file:%s,line:%d,errno:%d,%s",__FILE__,__LINE__,errno,strerror(errno));
									perror("11 != errno:");
									epoll_ctl(this->fd, EPOLL_CTL_DEL, fd, &ev);
								}	
    							break;
    						default:
    							{
    								buf[len] = '\0';
    								std::cout << buf << std::endl;
										int i = send(fd, buf, len, 0);
										std::cout << "send : " << i << std::endl;
								}
    							break;
						}
					}
					break;
				default:
					ALERT_LOG("FDINFO TYPE[TYPE=%u]", fdinfo.fd_type);
					break;
				}
			}

			if (evs[pos].events & EPOLLOUT) {
				//该套接字可写
				/*
				if (fdinfo.cb.sendlen > 0 && do_write_conn(fd) == -1) {
					//do_del_conn(fd, g_is_parent);
				}
				if (fdinfo.cb.sendlen == 0) {
					mod_events(this->fd, fd, EPOLLIN);
				}
				}*/
			}

			if (evs[pos].events & EPOLLHUP) {
				//do_del_conn(fd, g_is_parent);
			}
		}

		if (g_bench_conf.get_fd_time_out()) {
			for (int i = 0; i <= this->max_fd; ++i) {
				if ((g_net_server.m_fds[i].fd_type == fd_type_remote)
					&& ((time(0) - (int32_t)g_net_server.m_fds[i].last_tm) >= g_bench_conf.get_fd_time_out())) {
						//do_del_conn(i, g_is_parent);
				}
			}
		}

		g_dll.on_events();
	}
#endif
	return 0;
}