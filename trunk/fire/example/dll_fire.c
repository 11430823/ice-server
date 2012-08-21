
#include <lib_include.h>
#include <lib_log.h>
#include <bench_conf.h>
#include <interface.h>
#include <lib_timer.h>
#include <lib_proto/lib_proto.h>
#include <lib_proto/lib_msgbuf.h>
#include <lib_websocket/lib_websocket.h>
#include <lib_util.h>

#include "handle_cli.h"

ice::lib_websocket_t g_lib_websocket;
/*
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
	static int s_timer(void* owner, void* data){
		test_timer* pp = (test_timer*)owner;
		ADD_TIMER_EVENT(pp,&test_timer::s_timer, NULL,
			 ice::get_now_tv()->tv_sec+1);
		DEBUG_LOG("s_timer[%ld]", ice::get_now_tv()->tv_sec);
		return 0;
	}
	static int m_timer(void* owner, void* data){
		test_timer* pp = (test_timer*)owner;
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
ice::lib_timer_t g_timer;
*/
/**
  * @brief Initialize service
  *
  */
#include <lib_net/lib_udp.h>
extern "C" int on_init(int isparent)
{
	if(isparent){
		DEBUG_LOG("======daemon start======");
		std::string sip;
		ice::lib_net_util_t::get_ip_addr("eth0", AF_INET, sip);
		DEBUG_LOG("======%s,%lu======", sip.c_str(), sip.size());
		
		ice::lib_net_util_t::get_local_ip(sip);
		DEBUG_LOG(" %s", sip.c_str());
		ice::lib_udp_t udp;
		udp.connect("192.168.0.102", 8001);
		int num = udp.send("1234567", 7);
		DEBUG_LOG("udp send num=%u", num);
	}else{
		DEBUG_LOG("======server start======");
		//p = new test_timer;

		//test g_slice_alloc
		struct a 
		{
			int i;
			int j;
			int k;
		};

		/*
		for (int j = 0; j < 10; j++){
			ice::renew_now();
			uint32_t t_b = ice::get_now_tv()->tv_sec;

			for (int i = 0; i < 2000000000; i++){
				//a* aa = (a*)g_slice_alloc(sizeof(a));
				a* aa = (a*)malloc(sizeof(a));
				aa->i = 1;
				aa->j = 2;
				aa->k = 3;
				//g_slice_free1(sizeof(a), aa);
				free(aa);
			}

			ice::renew_now();
			uint32_t t_e = ice::get_now_tv()->tv_sec;
			DEBUG_LOG("todo test %u", t_e - t_b);
		}
		*/

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
		//delete p;
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
		//ice::lib_timer_t::handle_timer();
	}
}

/**
  * @brief Return length of the receiving package
  *
  */
extern "C" int on_get_pkg_len(ice::lib_tcp_peer_info_t* cli_fd_info, const void* data, uint32_t len)
{
	if (len < 4){
		return 0;
	}
	
// 	int ret = ice::lib_websocket_t::is_full_pack((char*)data, len); 
// 	if (0 == ret){
// 		TRACE_LOG("");
// 		return len;
// 	} else if (-1 == ret){
// 		TRACE_LOG("");
// 		return ret;
// 	}

	ice::lib_recv_data_cli_t in(data);
	uint32_t pkg_len = in.get_len();
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

	std::string outbuf;
	ice::bin2hex(outbuf, (char*)pkg, pkglen);
	TRACE_LOG("on_cli_pkg[len:%d, %s]", pkglen, outbuf.c_str());

// 	if (0 == g_lib_websocket.proc((char*)pkg, pkglen)){
// 		TRACE_LOG("xxxxxxxx");
// 		fire::s2peer(peer_fd_info, g_lib_websocket.get_websocket_buf(), g_lib_websocket.get_websocket_len());
// 		return 0;
// 	}
	ice::proto_head_t* head = (ice::proto_head_t*)pkg;
	TRACE_LOG("[len:%u, cmd:%u, seq:%u, ret:%u, uid:%u, fd:%d, pkglen:%d]",
		head->len, head->cmd, head->seq, head->ret, head->id, peer_fd_info->get_fd(), pkglen);
	return g_handle_cli.handle(pkg, pkglen);
	//fire::s2peer(peer_fd_info, pkg, pkglen);
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

