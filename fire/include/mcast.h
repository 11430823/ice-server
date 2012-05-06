/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		组播
*********************************************************************/

#pragma once

#include <map>
#include <time.h>

#include <lib_net/lib_multicast.h>

#pragma pack(1)
struct mcast_pkg_header_t {
	uint32_t	cmd;   // for mcast_notify_addr: 1st, syn
	char		body[];
	mcast_pkg_header_t();
};

enum E_MCAST_CMD{
	MCAST_CMD_ADDR_1ST	= 1,//启动时第一次发包
	MCAST_CMD_ADDR_SYN	= 2//平时同步用包
};

struct mcast_cmd_addr_1st_t {
	uint32_t	svr_id;
	char		name[16];
	char		ip[16];
	uint16_t	port;
	mcast_cmd_addr_1st_t();
};
typedef mcast_cmd_addr_1st_t mcast_cmd_addr_syn_t;

#pragma pack()

class addr_mcast_t : public ice::lib_mcast_t
{
public:
	addr_mcast_t();
	virtual ~addr_mcast_t(){}
	void mcast_notify_addr(E_MCAST_CMD pkg_type = MCAST_CMD_ADDR_SYN);
	void syn_info();
	void handle_msg(ice::lib_active_buf_t& fd_info);
	bool get_1st_svr_ip_port(const char* svr_name, std::string& ip, uint16_t& port);
protected:
private:
	addr_mcast_t(const addr_mcast_t& cr);
	addr_mcast_t& operator=(const addr_mcast_t& cr);

	struct addr_mcast_pkg_t {
		char		ip[16];
		uint16_t	port;
		uint32_t syn_time;
		addr_mcast_pkg_t();
	};
	mcast_pkg_header_t hdr;
	time_t next_notify_sec;//下一次通知信息的时间
	typedef std::map<uint32_t, addr_mcast_pkg_t> ADDR_MCAST_SVR_MAP;//KEY:svr_id,  val:svr_info
	typedef std::map<std::string, ADDR_MCAST_SVR_MAP> ADDR_MCAST_MAP;
public:
	ADDR_MCAST_MAP addr_mcast_map;//地址广播信息
	void add_svr_info(mcast_cmd_addr_1st_t& svr);
};

extern addr_mcast_t g_addr_mcast;
extern	ice::lib_mcast_t g_mcast;