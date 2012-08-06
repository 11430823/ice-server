#include <string.h>

#include <lib_timer.h>
#include <lib_random.h>

#include "dll.h"
#include "bind_conf.h"
#include "service.h"
#include "mcast.h"

addr_mcast_t g_addr_mcast;

namespace {
	const uint32_t ADDR_MCAST_SYN_TIME_OUT_SEC = 60;//同步地址超时秒数
}//end namespace 

mcast_pkg_header_t::mcast_pkg_header_t()
{
	this->cmd = 0;
}

void addr_mcast_t::mcast_notify_addr( E_MCAST_CMD pkg_type )
{
	mcast_cmd_addr_1st_t pkg;

	char* data = new char[sizeof(this->hdr) + sizeof(pkg)];
	this->hdr.cmd = pkg_type;
	pkg.svr_id     = g_service.bind_elem->id;
	strcpy(pkg.name, g_service.bind_elem->name.c_str());
	strcpy(pkg.ip, g_service.bind_elem->ip.c_str());
	pkg.port = g_service.bind_elem->port;
	memcpy(data, &(this->hdr), sizeof(this->hdr));
	memcpy(data + sizeof(this->hdr), &(pkg), sizeof(pkg));
	this->send(data, sizeof(this->hdr) + sizeof(pkg));
	SAFE_DELETE_ARR(data);

	this->next_notify_sec = ice::lib_random_t::random(20, 40) + ice::get_now_tv()->tv_sec;
}

void addr_mcast_t::syn_info()
{
	//清理过期的地址同步信息
	ADDR_MCAST_MAP::iterator it = this->addr_mcast_map.begin();
	for (; this->addr_mcast_map.end() != it;){
		std::string svr_name = it->first;
		ADDR_MCAST_SVR_MAP& info = it->second;
		for (ADDR_MCAST_SVR_MAP::iterator it2 = info.begin(); info.end() != it2;){
			uint32_t svr_id = it2->first;
			addr_mcast_pkg_t& syn = it2->second;
			if ((ice::get_now_tv()->tv_sec - syn.syn_time) > ADDR_MCAST_SYN_TIME_OUT_SEC){
				g_dll.functions.on_addr_mcast_pkg(svr_id, svr_name.c_str(),
					syn.ip, syn.port, 0);
				info.erase(it2++);
			} else {
				it2++;
			}
		}
		if (info.empty()){
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
	::srand(::getpid());
}

void addr_mcast_t::handle_msg(ice::lib_active_buf_t& recv_buf)
{
	mcast_pkg_header_t* hdr = (mcast_pkg_header_t*)recv_buf.get_data();
	mcast_cmd_addr_1st_t* add_mcast_info = (mcast_cmd_addr_1st_t*)hdr->body;
	std::string strname = add_mcast_info->name;
	ADDR_MCAST_MAP::iterator it = this->addr_mcast_map.find(strname);
	if (add_mcast_info->name == g_service.bind_elem->name
		&& add_mcast_info->svr_id == g_service.bind_elem->id){
		return;
	}

	TRACE_LOG("cmd:%u, ip:%s, name:%s, port:%u, id:%u", hdr->cmd,
		add_mcast_info->ip, add_mcast_info->name, add_mcast_info->port, add_mcast_info->svr_id);
	switch (hdr->cmd)
	{
	case MCAST_CMD_ADDR_1ST:
		{
			if (this->addr_mcast_map.end() != it 
				&& it->second.end() != it->second.find(add_mcast_info->svr_id)){
					ALERT_LOG("addr mcast svr name conflict [name:%s, ip:%s, port:%u, id:%u]", 
						add_mcast_info->name, add_mcast_info->ip, add_mcast_info->port, add_mcast_info->svr_id);
					return;
			} else {
				this->add_svr_info(*add_mcast_info);
				this->mcast_notify_addr();
			}
			g_net_server.get_server_epoll()->get_on_functions()->on_addr_mcast_pkg(add_mcast_info->svr_id, 
				add_mcast_info->name, add_mcast_info->ip, add_mcast_info->port, 1);
		}
		break;
	case MCAST_CMD_ADDR_SYN:
		{
			if (this->addr_mcast_map.end() != it
				&& it->second.end() != it->second.find(add_mcast_info->svr_id)){
					addr_mcast_pkg_t& syn_info = it->second[add_mcast_info->svr_id];
					syn_info.syn_time = ice::get_now_tv()->tv_sec;
			} else {
				this->add_svr_info(*add_mcast_info);
				this->mcast_notify_addr();
			}
			g_net_server.get_server_epoll()->get_on_functions()->on_addr_mcast_pkg(add_mcast_info->svr_id, 
				add_mcast_info->name, add_mcast_info->ip, add_mcast_info->port, 1);
		}
		break;
	default:
		ERROR_LOG("cmd:%u, ip:%s, name:%s, port:%u, id:%u", hdr->cmd,
			add_mcast_info->ip, add_mcast_info->name, add_mcast_info->port, add_mcast_info->svr_id);
		return;
		break;
	}
}

void addr_mcast_t::add_svr_info( mcast_cmd_addr_1st_t& svr )
{
	addr_mcast_pkg_t syn_info;
	strcpy(syn_info.ip, svr.ip);
	syn_info.port = svr.port;
	this->addr_mcast_map[svr.name][svr.svr_id] = syn_info;
}

bool addr_mcast_t::get_1st_svr_ip_port( const char* svr_name, std::string& ip, uint16_t& port )
{
	FOREACH(this->addr_mcast_map, it){
		if (it->first == svr_name){
			ADDR_MCAST_SVR_MAP& am = it->second;
			FOREACH(am, it2){
				addr_mcast_pkg_t& ampt = it2->second;
				ip = ampt.ip;
				port = ampt.port;
				return true;
			}
		}
	}
	return false;
}

mcast_cmd_addr_1st_t::mcast_cmd_addr_1st_t()
{
	memset(this, 0, sizeof(*this));
}

addr_mcast_t::addr_mcast_pkg_t::addr_mcast_pkg_t()
{
	memset(this->ip, 0, sizeof(this->ip));
	this->syn_time = ice::get_now_tv()->tv_sec;
}
