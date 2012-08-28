

#pragma once

#include <lib_include.h>
#include <lib_util.h>
#include <lib_msgbuf.h>

#include "proto_header.h"

class Cfunc_route;
#define  DEAL_FUN_ARG const ice::proto_head_t& head, ice::lib_recv_data_cli_t& in, char** sendbuf, int& sndlen
//定义调用函数的指针类型
typedef int (Cfunc_route::*P_DEALFUN_T)(DEAL_FUN_ARG);

class Ccmdmap
{
private:
	typedef std::map<uint32_t, P_DEALFUN_T> CMD_MAP;
	CMD_MAP cmd_map;
public:
	P_DEALFUN_T get_cmd_fun(uint32_t cmd){
		CMD_MAP::iterator it = this->cmd_map.find(cmd);
		if (this->cmd_map.end() != it){
			return it->second;
		}
		return NULL;
	}

	void insert_cmd_fun(uint32_t cmd, P_DEALFUN_T deal_fun){
		cmd_map[cmd] = deal_fun;
	}
};

#define SET_SNDBUF(private_len)  \
	if (!(set_std_return (sendbuf,sndlen,  (PROTO_HEADER*)recvbuf, SUCC, (private_len)))){ \
		DEBUG_LOG("set_std_return:private size err [%d]",int(private_len));\
		return SYS_ERR;\
	}

//没有私有返回数据时，可用 在Croute_func 的 相关处理函数中使用
#define STD_RETURN(ret) {\
	if (ret==SUCC){  \
		SET_SNDBUF(0);\
	}\
	return ret;\
}

#define STD_RETURN_WITH_BUF(ret,_buf,_count ) { \
	if (ret==SUCC){ \
		SET_SNDBUF( (_count) ); \
		memcpy(SNDBUF_PRIVATE_POS, (char*)(_buf) , (_count)  ); \
	}\
	return ret; \
}

#define STD_RETURN_WITH_TWO_BUF(ret,_buf1,_buf1_len,_buf2,_buf2_len) { \
	if (ret==SUCC){ \
		SET_SNDBUF( (_buf1_len)+(_buf2_len) ); \
		memcpy(SNDBUF_PRIVATE_POS, (char*)(_buf1) , (_buf1_len)  ); \
		memcpy(SNDBUF_PRIVATE_POS+_buf1_len, (char*)(_buf2) , (_buf2_len)  ); \
	}\
	return ret; \
}


//在有私有数据时采用， stru 为 返回的私有结构体。
#define STD_RETURN_WITH_STRUCT_AND_BUF(ret,stru,_buf,_buf_len ) STD_RETURN_WITH_TWO_BUF (ret,((char*)&stru), sizeof(stru),_buf,_buf_len  )



//在有私有数据时采用， stru 为 返回的私有结构体。
#define STD_RETURN_WITH_STRUCT(ret,stru ) STD_RETURN_WITH_BUF (ret,((char*)&stru), sizeof(stru)  )



//如果列表malloc
#ifndef DB_GET_LIST_NO_MALLOC  
//在有返回列表 时采用， 
//ret:返回值  
//stru_header:结构体头部
//pstru_item:列表起始指针
//itemcount: 个数
//注意事项：pstru_item所指向的空间应是molloc得到的,:free(pstru_item);
#define STD_RETURN_WITH_STRUCT_LIST(ret,stru_header,pstru_item ,itemcount) \
	if (ret==SUCC){ \
		uint32_t stru_header_size=sizeof(stru_header );\
		uint32_t out_list_size= sizeof(*pstru_item ) * (itemcount);\
		SET_SNDBUF(stru_header_size + out_list_size);\
		memcpy(SNDBUF_PRIVATE_POS,&stru_header,stru_header_size);\
		memcpy(SNDBUF_PRIVATE_POS+stru_header_size,pstru_item, out_list_size );\
		free(pstru_item);\
		return SUCC;\
	} else {\
		return ret;\
	} 


#else

#define STD_RETURN_WITH_STRUCT_LIST(ret,stru_header,pstru_item ,itemcount) \
	if (ret==SUCC){ \
		uint32_t stru_header_size=sizeof(stru_header );\
		uint32_t out_list_size= sizeof(*pstru_item ) * (itemcount);\
		SET_SNDBUF(stru_header_size + out_list_size);\
		memcpy(SNDBUF_PRIVATE_POS,&stru_header,stru_header_size);\
		memcpy(SNDBUF_PRIVATE_POS+stru_header_size,pstru_item, out_list_size );\
		return SUCC;\
	} else {\
		return ret;\
	} 
#endif
