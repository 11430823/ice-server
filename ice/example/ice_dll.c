#include <stdint.h>
#include <stdio.h>


#include <ice_dll.h>

#include <lib_log.h>
#include <lib_timer.h>




class test_timer;
test_timer* p;

class test_timer
{
public:
		static const int mmm= 1;
public:
	test_timer(void){
			/*
		DEBUG_LOG("new");
		INIT_LIST_HEAD(&timer_list);
		ADD_TIMER_EVENT(this, &test_timer::s_timer,NULL,
			get_now_tv()->tv_sec + 1);
		timeval next_time;
		next_time.tv_sec = get_now_tv()->tv_sec;
		next_time.tv_usec = get_now_tv()->tv_usec + 300000;
		//add_micro_event(&test_timer::m_timer, &next_time, this, NULL);
		*/
	}
	virtual ~test_timer(){
			/*
		DEBUG_LOG("del");
		REMOVE_TIMERS(this);
		*/
	}
protected:
	
private:
	list_head_t timer_list;
	static int s_timer(void* data, void* info){
			/*
		test_timer* pPlantManager = (test_timer*)data;
		const uint32_t uNowTimeS = get_now_tv()->tv_sec;
		ADD_TIMER_EVENT(pPlantManager,&test_timer::s_timer, NULL,
			uNowTimeS+1);
		DEBUG_LOG("s_timer[%u]",uNowTimeS);
		delete p;
		p = new test_timer;
		*/
		return 0;
	}
	static int m_timer(void* data, void* info){
			/*
		test_timer* pPlantManager = (test_timer*)data;
		timeval next_time;
		next_time.tv_sec = get_now_tv()->tv_sec;
		next_time.tv_usec = get_now_tv()->tv_usec + 300000;
		add_micro_event(&test_timer::m_timer, &next_time, pPlantManager, NULL);
//		DEBUG_LOG("m_timer[%ld, %ld]", next_time.tv_sec, next_time.tv_usec);
*/
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
	}else{
		DEBUG_LOG("======server start======");
		/*
		setup_timer();
		p = new test_timer;
		std::string str1;
		std::string str2;
		std::string strval = g_bench_conf.get_strval(str1, str2);
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
	if (!isparent) {
			/*
		delete p;
		destroy_timer();
		*/
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
	//handle_timer();
}

/**
  * @brief Return length of the receiving package
  *
  */
extern "C" int on_get_pkg_len(int fd, const void* avail_data, int avail_len, int isparent)
{
	if (avail_len < 4) {
		return 0;
	}
	uint32_t len = 0;
/*
	cli_proto_head_t* head = (cli_proto_head_t *)avail_data;
	len = ice::bswap(head->len);
	TRACE_LOG("[len:%u]", len);
	if (isparent) {
		if (len > g_bench_conf.get_m_page_size_max() || len < (uint32_t)sizeof(cli_proto_head_t)) {
			CRIT_LOG("[fd:%d, avail_len:%d, isparent:%d, len:%u, page_size_max:%u]",
				fd, avail_len, isparent, len, g_bench_conf.get_m_page_size_max());
			return -1;
		}
	}

*/
	INFO_LOG("[fd:%d, avail_len:%d, isparent:%d, avail_data:%s]", fd, avail_len, isparent, (char*)avail_data);
	return len;
}

/**
  * @brief Process packages from clients
  *
  */
extern "C" int on_cli_pkg(void* data, int len, fdsession_t* fdsess)
{
	/* 返回非零，断开FD的连接 */ 
	INFO_LOG("[fd:%d, len:%d, data:%s]", fdsess->fd, len, (char*)data);
/*
	cli_proto_head_t* p = (cli_proto_head_t*)data;
	INFO_LOG("[cmd:%d, id:%d, len:%d, ret:%d, seq_num:%d]", p->cmd, p->id, p->len, p->ret, p->seq_num);
	
	send_pkg_to_client(fdsess, data, len);
*/
	return 0;
}

/**
  * @brief Process packages from servers
  *
  */
extern "C" void on_srv_pkg(int fd, void* data, int len)
{
}

/**
  * @brief Called each time on client connection closed
  *
  */
extern "C" void on_cli_conn_closed(int fd)
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
extern "C" void on_mcast_pkg(void* data, int len)
{
}

extern "C" int	on_udp_pkg(int fd, const void* avail_data, int avail_len ,struct sockaddr_in * from, socklen_t fromlen )
{
	return 0;
}

extern "C" void on_sync_srv_info(uint32_t svr_id, const char* svr_name, const char* svr_ip, in_port_t port, int flag)
{
}



