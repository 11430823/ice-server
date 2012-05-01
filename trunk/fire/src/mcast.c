#include <string.h>

#include <lib_timer.h>
#include <lib_random.h>

#include "dll.h"
#include "bind_conf.h"
#include "service.h"
#include "mcast.h"

addr_mcast_t g_addr_mcast;
ice::lib_mcast_t g_mcast;

namespace {
	const uint32_t ADDR_MCAST_SYN_TIME_OUT_SEC = 60;//同步地址超时秒数
}//end namespace 

mcast_pkg_header_t::mcast_pkg_header_t()
{
	this->pkg_type = 0;
	this->proto_type = 0;
}

void addr_mcast_t::mcast_notify_addr( E_ADDR_MCAST_PKG_TYPE pkg_type )
{
	char* data = new char[sizeof(this->hdr) + sizeof(this->pkg)];
	this->hdr.pkg_type = pkg_type;
	this->hdr.proto_type = MCAST_NOTIFY_ADDR;
	this->pkg.svr_id     = g_service.bind_elem->id;
	strcpy(this->pkg.name, g_service.bind_elem->name.c_str());
	strcpy(this->pkg.ip, g_service.bind_elem->ip.c_str());
	this->pkg.port = g_service.bind_elem->port;
	memcpy(data, &(this->hdr), sizeof(this->hdr));
	memcpy(data + sizeof(this->hdr), &(this->pkg), sizeof(this->pkg));
	this->send(data, sizeof(this->hdr) + sizeof(this->pkg));
	SAFE_DELETE_ARR(data);

	this->next_notify_sec = ice::lib_random_t::random(20,30) + ice::get_now_tv()->tv_sec;
}

void addr_mcast_t::syn_info()
{
	//清理过期的地址同步信息
	ADDR_MCAST_MAP::iterator it = this->addr_mcast_map.begin();
	for (; this->addr_mcast_map.end() != it;){
		addr_mcast_syn_info_t& info = it->second;
		if (ice::get_now_tv()->tv_sec - info.syn_time > ADDR_MCAST_SYN_TIME_OUT_SEC){
			g_dll.functions.on_addr_mcast_pkg(info.addr_mcast_info.svr_id, info.addr_mcast_info.name,
				info.addr_mcast_info.ip, info.addr_mcast_info.port, 0);
			this->addr_mcast_map.erase(it++);
		} else {
			it++;
		}
	}

	//定时广播自己的地址信息
	if (this->next_notify_sec < ice::get_now_tv()->tv_sec){
		this->mcast_notify_addr();
	}
}

addr_mcast_t::addr_mcast_t()
{
	this->next_notify_sec = 0;
}


addr_mcast_pkg_t::addr_mcast_pkg_t()
{
	this->svr_id = 0;
	memset(this->name, 0, sizeof(this->name));
	memset(this->ip, 0, sizeof(this->ip));
	this->port = 0;
}

addr_mcast_t::addr_mcast_syn_info_t::addr_mcast_syn_info_t()
{
	syn_time = ice::get_now_tv()->tv_sec;
}
