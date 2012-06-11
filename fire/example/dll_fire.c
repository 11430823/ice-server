#include <stdint.h>
#include <stdio.h>

#include <lib_log.h>
#include <bench_conf.h>
#include <interface.h>

#include <lib_timer.h>
#include <lib_proto.h>
#include <lib_msgbuf.h>

class test_timer;
test_timer* p;

class test_timer
{
public:
		static const int mmm= 1;
public:
	test_timer(void){
		INIT_LIST_HEAD(&timer_list);
		ADD_TIMER_EVENT(this, &test_timer::s_timer,NULL, ice::get_now_tv()->tv_sec + 1);

		timeval next_time;
		next_time.tv_sec = ice::get_now_tv()->tv_sec;
		next_time.tv_usec = ice::get_now_tv()->tv_usec + 300000;
		ice::add_micro_event(&test_timer::m_timer, &next_time, this, NULL);
	}
	virtual ~test_timer(){
		REMOVE_TIMERS(this);
	}
protected:
	
private:
	list_head_t timer_list;
	static int s_timer(void* data, void* info){
		test_timer* pp = (test_timer*)data;
		ADD_TIMER_EVENT(pp,&test_timer::s_timer, NULL,
			 ice::get_now_tv()->tv_sec+1);
		DEBUG_LOG("s_timer[%ld]", ice::get_now_tv()->tv_sec);
		return 0;
	}
	static int m_timer(void* data, void* info){
		test_timer* pp = (test_timer*)data;
		timeval next_time;
		next_time.tv_sec = ice::get_now_tv()->tv_sec;
		next_time.tv_usec = ice::get_now_tv()->tv_usec + 300000;
		ice::add_micro_event(&test_timer::m_timer, &next_time, pp, NULL);
		DEBUG_LOG("m_timer[%ld]", ice::get_now_tv()->tv_usec);
		return 0;
	}
	test_timer(const test_timer &cr);
	test_timer & operator=( const test_timer &cr);
};

/**
  * @brief Initialize service
  *
  */
extern "C" int on_init(int isparent)
{
	if(isparent){
		DEBUG_LOG("======daemon start======");
	}else{
		DEBUG_LOG("======server start======");
		ice::setup_timer();
		p = new test_timer;
	}
	return 0;
}

/**
  * @brief Finalize service
  *
  */
extern "C" int on_fini(int isparent)
{
	if (isparent) {
		DEBUG_LOG("======daemon done======");
	}else{
		delete p;
		ice::destroy_timer();
		DEBUG_LOG("======server done======");
	}
	return 0;
}

/**
  * @brief Process events such as timers and signals
  *
  */
extern "C" void on_events()
{
	if (fire::is_parent()){
	}else{
		ice::handle_timer();
	}
}

//接收客户端的数据
class recv_data_t : public ice::lib_recv_data_t{
public:
	recv_data_t(const void* recvdata, int readpos = 0)
		:ice::lib_recv_data_t(recvdata, readpos){
	}
public:
	//前4个字节是整个包长度
	uint32_t get_len(){
		return (ice::lib_byte_swap_t::bswap((uint32_t)(*(uint32_t*)this->recv_data)));
	}
	uint32_t remain_len() {
		return get_len() - this->read_pos;
	}
protected:
private:
	recv_data_t(const recv_data_t& cr); // 拷贝构造函数
	recv_data_t& operator=( const recv_data_t& cr); // 赋值函数
};

/**
  * @brief Return length of the receiving package
  *
  */
extern "C" int on_get_pkg_len(ice::lib_tcp_peer_info_t* cli_fd_info, const void* data, uint32_t len)
{
	if (len < 4){
		return 0;
	}

	uint32_t pkg_len = 0;
	recv_data_t in(data);
	ice::lib_packer_t::unpack(data, pkg_len, )
	pkg_len = *(uint32_t*)(data);
	TRACE_LOG("[fd:%d, len:%d, pkg_len:%u]", cli_fd_info->get_fd(), len, pkg_len);
	if (pkg_len < sizeof(ice::proto_head_t) || pkg_len > g_bench_conf.get_page_size_max()){
		ERROR_LOG("head len err [len=%u]", pkg_len);
		return -1;
	}

	return pkg_len;
}

/**
  * @brief Process packages from clients
  *
  */
extern "C" int on_cli_pkg(const void* pkg, int pkglen, ice::lib_tcp_peer_info_t* peer_fd_info)
{
	/* 返回非零，断开FD的连接 */ 
	ice::proto_head_t* head = (ice::proto_head_t*)pkg;
	TRACE_LOG("[len:%u, cmd:%u, seq:%u, ret:%u, uid:%u, fd:%d, pkglen:%d]",
		head->len, head->cmd, head->seq, head->ret, head->id, peer_fd_info->get_fd(), pkglen);
	fire::s2peer(peer_fd_info, pkg, pkglen);
	return 0;
}

/**
  * @brief Process packages from servers
  *
  */
extern "C" void on_srv_pkg(const void* pkg, int pkglen, ice::lib_tcp_peer_info_t* peer_fd_info)
{
	TRACE_LOG("[fd:%d, ip:%s, port:%u]", peer_fd_info->get_fd(), peer_fd_info->get_ip_str().c_str(), peer_fd_info->get_port());
	fire::s2peer(peer_fd_info, pkg, pkglen);
}

/**
  * @brief Called each time on client connection closed
  *
  */
extern "C" void on_cli_conn_closed(int fd)
{
	TRACE_LOG("[fd%d]", fd);
}

/**
  * @brief Called each time on close of the fds created by the child process
  *
  */
extern "C" void on_svr_conn_closed(int fd)
{
	TRACE_LOG("[fd%d]", fd);
}

/**
 * @brief	Called to process multicast packages from the specified `mcast_ip` and `mcast_port`. Called once for each package.
 */
extern "C" void	on_mcast_pkg(const void* data, int len)
{
	TRACE_LOG("[len:%d]", len);
}

/**
 * @brief	Called to process multicast packages from the specified `add_mcast_ip` and `add_mcast_port`. Called once for each package.
 */
extern "C"  void on_addr_mcast_pkg(uint32_t id, const char* name, const char* ip, uint16_t port, int flag/*1:可用.0:不可用*/)
{
	TRACE_LOG("[id:%u, name:%s, ip:%s, port:%u, flag:%d]", id, name, ip, port, flag);
}

extern "C" void on_udp_pkg(int fd, const void* data, int len ,struct sockaddr_in* from, socklen_t fromlen)
{
}

