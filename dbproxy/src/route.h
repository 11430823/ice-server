
#include <map>

#include <lib_xmlparser.h>
#include <lib_util.h>

#pragma once

#pragma pack(1)
struct  route_cmd_t
{
	uint32_t start_id;
	uint32_t end_id;
	uint32_t now_id;
	route_cmd_t(){
		this->start_id = 0;
		this->end_id = 0;
		this->now_id = 0;
	}
	bool operator < (const route_cmd_t& r) const
	{
		return !(this->start_id <= r.now_id && r.now_id <= this->end_id);
	}
};

struct connect_db_t 
{
};
#pragma pack()

class route_t
{
	PRIVATE(ice::lib_xmlparser, xml);
	typedef 
	typedef std::map<route_cmd_t, connect_db_t> CMD_MAP;//key:cmd∑∂Œß, val:dbser¡–±Ì
	CMD_MAP cmd_map;
public:
	route_t(){}
	virtual ~route_t(){}
	int parser();
	void find_dbser(uint32_t cmd){
		route_cmd_t info;
		info.now_id = cmd;
		CMD_MAP::iterator it = cmd_map.find(info);
		//todo
		
	}
protected:
	
private:
	route_t(const route_t& cr);
	route_t& operator=(const route_t& cr);
};

extern route_t g_rotue_t;