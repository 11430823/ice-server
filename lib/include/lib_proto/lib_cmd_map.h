/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	ok
	brief:		命令消息存放
*********************************************************************/

#include "lib_include.h"
#include "lib_proto/lib_msg.h"

#pragma once

namespace ice{

	//命令数据
	template <typename T> 
	struct lib_cmd_t{
		uint32_t cmd_id;
		lib_msg_t* p_msg;
		T func;
		lib_cmd_t(){
			cmd_id = 0;
			p_msg = NULL;
		}
	};

	//存放命令消息,与映射的命令数据
	template <typename T> 
	class lib_cmd_map_t{
	public:
		lib_cmd_map_t(){}
		virtual ~lib_cmd_map_t(){}
// 		inline void insert(lib_cmd_t<T> item){
// 			assert(this->cmd_map.end() == this->cmd_map.find(item.cmd_id));
// 			this->cmd_map[item.cmd_id]= item;
// 		}
		inline void insert(uint32_t cmd_id, T func, lib_msg_t* p_msg){
			assert(this->cmd_map.end() == this->cmd_map.find(cmd_id));
			lib_cmd_t<T> item;
			item.cmd_id = cmd_id;
			item.p_msg = p_msg;
			item.func = func;
			this->cmd_map[cmd_id] = item;
		}

		inline lib_cmd_t<T>* get(uint32_t cmd_id){
			if (this->cmd_map.end() != this->cmd_map.find(cmd_id)){
				return  &this->cmd_map[cmd_id];
			}
			return NULL;
		}
	private:
		std::map<uint32_t, lib_cmd_t<T> > cmd_map;
		lib_cmd_map_t(const lib_cmd_map_t& cr);
		lib_cmd_map_t& operator=(const lib_cmd_map_t& cr);
	};
}//end namespace ice