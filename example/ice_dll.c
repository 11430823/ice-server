#include "../include/dll.h"
#include <stdint.h>


#include <stdio.h>

#include "log.h"
#include "timer.h"

class test_timer
{
public:
public:
	test_timer(void){
		INIT_LIST_HEAD(&timer_list);
		ADD_TIMER_EVENT(this, &test_timer::s_timer,NULL,
			get_now_tv()->tv_sec + 1);
		timeval next_time;
		next_time.tv_sec = get_now_tv()->tv_sec;
		next_time.tv_usec = get_now_tv()->tv_usec + 300000;
		add_micro_event(&test_timer::m_timer, &next_time, this, NULL);
	}
	virtual ~test_timer(){
		REMOVE_TIMERS(this);
	}
protected:
	
private:
	list_head_t timer_list;
	static int s_timer(void* data, void* info){
		test_timer* pPlantManager = (test_timer*)data;
		const uint32_t uNowTimeS = get_now_tv()->tv_sec;
		ADD_TIMER_EVENT(pPlantManager,&test_timer::s_timer, NULL,
			uNowTimeS+1);
		DEBUG_LOG("s_timer[%u]",uNowTimeS);
		return 0;
	}
	static int m_timer(void* data, void* info){
		test_timer* pPlantManager = (test_timer*)data;
		timeval next_time;
		next_time.tv_sec = get_now_tv()->tv_sec;
		next_time.tv_usec = get_now_tv()->tv_usec + 300000;
		add_micro_event(&test_timer::m_timer, &next_time, pPlantManager, NULL);
		DEBUG_LOG("m_timer[%ld, %ld]", next_time.tv_sec, next_time.tv_usec);
		return 0;
	}
	test_timer(const test_timer &cr);
	test_timer & operator=( const test_timer &cr);
};
test_timer* p;
/**
  * @brief Initialize service
  *
  */
extern "C" int init_service(int isparent)
{
	if(isparent){
	}else{
		DEBUG_LOG("======server start======");
		setup_timer();
		p = new test_timer;
	}
	return 0;
}

/**
  * @brief Finalize service
  *
  */
extern "C" int fini_service(int isparent)
{
	if (!isparent) {
		delete p;
		destroy_timer();
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
	handle_timer();
}

/**
  * @brief Return length of the receiving package
  *
  */
extern "C" int get_pkg_len(int fd, const void* avail_data, int avail_len, int isparent)
{
	if (avail_len < 4) {
		return 0;
	}

	uint32_t len = *(uint32_t *)avail_data;
// 	if (isparent) {
// 		if (len > e_msg_len_max || len < (int)sizeof(btl_proto_head_t)) {
// 			return -1;
// 		}
// 	}
	return len;
}

/**
  * @brief Process packages from clients
  *
  */
extern "C" int proc_pkg_from_client(void* data, int len, fdsession_t* fdsess)
{
	/* 返回非零，断开FD的连接 */ 
	return 0;
}

/**
  * @brief Process packages from servers
  *
  */
extern "C" void proc_pkg_from_serv(int fd, void* data, int len)
{
}

/**
  * @brief Called each time on client connection closed
  *
  */
extern "C" void on_client_conn_closed(int fd)
{
}

/**
  * @brief Called each time on close of the fds created by the child process
  *
  */
extern "C" void on_fd_closed(int fd)
{
}

/**
  * @brief Called to process mcast package from the address and port configured in the config file
  */
extern "C" void proc_mcast_pkg(void* data, int len)
{
}

/* *
 * @brief Called to reload global data(mainly global function address.etc)
 */
extern "C" int reload_global_data()
{
	return 0;
}

extern "C" int	proc_udp_pkg(int fd, const void* avail_data, int avail_len ,struct sockaddr_in * from, socklen_t fromlen )
{
	return 0;
}

extern "C" void sync_service_info(uint32_t svr_id, const char* svr_name, const char* svr_ip, in_port_t port, int flag)
{
}



