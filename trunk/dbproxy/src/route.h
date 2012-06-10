#pragma once

#include <map>

#include <lib_xmlparser.h>
#include <lib_util.h>

#pragma pack(1)
struct  route_cmd_t
{
	uint32_t start;
	uint32_t end;
	uint32_t now;
	uint32_t db_type;
	route_cmd_t(){
		this->start = 0;
		this->end = 0;
		this->now = 0;
		this->db_type = 0;
	}
	bool operator < (const route_cmd_t& r) const
	{
		return !(this->start <= r.now && r.now <= this->end);
	}
};

struct route_db_t
{
	uint32_t start;
	uint32_t end;
	uint32_t now;
	route_cmd_t(){
		this->start = 0;
		this->end = 0;
		this->now = 0;
	}
	bool operator < (const route_db_t& r) const
	{
		return !(this->start <= r.now && r.now <= this->end);
	}
};

struct db_info_t 
{
	std::string name;
	std::string ip;
};
#pragma pack()

enum e_db_type{
	E_DB_TYPE_1 = 1,
	E_DB_TYPE_100 = 100,
};

class route_t
{
	PRIVATE(ice::lib_xmlparser, xml);
	typedef std::map<route_db_t, db_info_t> DB_SER;
	typedef std::map<route_cmd_t, DB_SER> CMD_MAP;//key:cmd·¶Î§, val:dbserÁÐ±í
	CMD_MAP cmd_map;
public:
	route_t(){}
	virtual ~route_t(){}
	int parser();
	DB_SER* find_dbser(uint32_t cmd, uint32_t& db_type){
		route_cmd_t info;
		info.now = cmd;
		CMD_MAP::iterator it = cmd_map.find(info);
		if (cmd_map.end() != it){
			db_type = it->first.db_type;
			return it->second;
		}
		return NULL;
	}
	db_info_t* find_dbinfo(uint32_t id, DB_SER* dbser, uint32_t db_type){
		route_db_t info;
		info.now = id;
		DB_SER::iterator it = dbser->find(info);
		if (dbser->end() != it){
			db_info_t& info = it->second;
		}

		return NULL;
	}

protected:
	
private:
	route_t(const route_t& cr);
	route_t& operator=(const route_t& cr);
};

extern route_t g_rotue_t;
