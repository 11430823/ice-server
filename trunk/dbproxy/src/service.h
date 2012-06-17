/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		·þÎñ
*********************************************************************/

#pragma once

class service_t
{
public:
	service_t(){
		this->init();
	}
	//virtual ~service_t(){}
	int init(){
		this->name.clear();
		this->peer = NULL;
		return 0;
	}
	std::string name;
	ice::lib_tcp_peer_info_t* peer;	
	service_t(const service_t& cr){
		this->name = cr.name;
		this->peer = cr.peer;
	}
protected:
	
private:

	service_t& operator=(const service_t& cr);

};

class service_mgr_t
{
public:
	service_mgr_t(){}
	//virtual ~service_mgr_t(){}
	int add(std::string name, ice::lib_tcp_peer_info_t* peer){
		SERVICE_MAP::iterator it = this->service_map.find(peer->get_fd());
		if (this->service_map.end() != it){
			return -1;
		}

		SERVICE_NAME_MAP::iterator it_name = this->service_name_map.find(name);
		if (this->service_name_map.end() != it_name){
			return -1;
		}

		service_t info;
		info.name = name;
		info.peer = peer;
		this->service_map.insert(std::make_pair(peer->get_fd(), info));
		this->service_name_map.insert(std::make_pair(name, info));
		return 0;
	}
	int remove(int fd){
		SERVICE_MAP::iterator it = this->service_map.find(fd);
		if (this->service_map.end() != it){
			return -1;
		}
		std::string name = it->second.name;

		this->service_map.erase(fd);
		this->service_name_map.erase(name);

		return 0;
	}

	service_t* get_service(std::string name){
		SERVICE_NAME_MAP::iterator it = this->service_name_map.find(name);
		if (this->service_name_map.end() != it){
			return &it->second;
		}
		return NULL;
	}
	
protected:
	
private:
	service_mgr_t(const service_mgr_t& cr);
	service_mgr_t& operator=(const service_mgr_t& cr);
	typedef std::map<int, service_t> SERVICE_MAP;
	SERVICE_MAP service_map;
	typedef std::map<std::string, service_t> SERVICE_NAME_MAP;
	SERVICE_NAME_MAP service_name_map;
};

extern service_mgr_t g_service_mgr;