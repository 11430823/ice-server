/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		命令消息存放
*********************************************************************/

#include "lib_include.h"

#pragma once

namespace ice{

	//命令数据
	template <typename T> 
	struct lib_cmd_t{
		uint32_t cmdid;
		T func;
	};

	//存放命令消息,与映射的命令数据
	template <typename T> 
	class lib_cmd_map_t{
		lib_cmd_map_t(){}
		virtual ~lib_cmd_map_t(){}
		inline void insert(lib_cmd_t<T> item){
			assert(this->cmdmap.end() == this->cmdmap.find(item.cmdid));
			this->cmdmap[item.cmdid]= item;
		}

		inline lib_cmd_t<T>* get(uint32_t cmdid){
			if (this->cmdmap.end() != this->cmdmap.find(cmdid)){
				return  &this->cmdmap[cmdid];
			}
			return NULL;
		}
	private:
		std::map<uint32_t, lib_cmd_t<T>> cmdmap;
		lib_cmd_map_t(const lib_cmd_map_t& cr);
		lib_cmd_map_t& operator=(const lib_cmd_map_t& cr);
	};
}//end namespace ice