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
	uint16_t	pkg_type;   // for mcast_notify_addr: 1st, syn
	uint16_t	proto_type; // mcast_notify_addr...
	char		body[];
	mcast_pkg_header_t();
};

struct addr_mcast_pkg_t {
	uint32_t	svr_id;
	char		name[16];
	char		ip[16];
	uint16_t	port;
	addr_mcast_pkg_t();
};
#pragma pack()

enum E_MCAST_NOTIFY_TYPE{
	MCAST_NOTIFY_ADDR   = 0,
};

class addr_mcast_t : public ice::lib_mcast_t
{
public:
	enum E_ADDR_MCAST_PKG_TYPE{
		ADDR_MCAST_1ST_PKG	= 1,//启动时第一次发包
		ADDR_MCAST_SYN_PKG	= 2//平时同步用包
	};
	struct addr_mcast_syn_info_t 
	{
		addr_mcast_pkg_t addr_mcast_info;
		uint32_t syn_time;
		addr_mcast_syn_info_t();
	};
	typedef std::map<std::string, addr_mcast_syn_info_t> ADDR_MCAST_MAP;
	ADDR_MCAST_MAP addr_mcast_map;//地址广播信息

public:
	addr_mcast_t();
	virtual ~addr_mcast_t(){}
	void mcast_notify_addr(E_ADDR_MCAST_PKG_TYPE pkg_type = ADDR_MCAST_SYN_PKG);
	void syn_info();
protected:

private:
	addr_mcast_t(const addr_mcast_t& cr);
	addr_mcast_t& operator=(const addr_mcast_t& cr);
	mcast_pkg_header_t hdr;
	addr_mcast_pkg_t   pkg;
	time_t next_notify_sec;//下一次通知信息的时间


};

extern addr_mcast_t g_addr_mcast;
extern	ice::lib_mcast_t g_mcast;