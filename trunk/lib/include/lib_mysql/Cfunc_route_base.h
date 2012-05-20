/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		
*********************************************************************/

#pragma once

#include <lib_util.h>
#include <lib_msgbuf.h>

#include "mysql_iface.h"
#include "Cfunc_route_cmd.h"



//////////////////////////////////////////////////////////////////////////
//接收客户端的数据
class recv_data_cli_t : public ice::lib_recv_data_t{
public:
	recv_data_cli_t(const void* recvdata, int readpos = 0)
		:ice::lib_recv_data_t(recvdata, readpos){
	}
public:
	//前4个字节是整个包长度
	uint32_t get_len(){
		return (ice::lib_byte_swap_t::bswap((uint32_t)(*(uint32_t*)this->recv_data)));
	}
	uint32_t remain_len() {
		return get_len() - this->read_pos;
	}
protected:
private:
	recv_data_cli_t(const recv_data_cli_t& cr); // 拷贝构造函数
	recv_data_cli_t& operator=( const recv_data_cli_t& cr); // 赋值函数
};

class Cfunc_route_base : public Cfunc_route_cmd
{
	PROTECTED_R_DEFAULT(mysql_interface*, db);//db 连接
public:
	Cfunc_route_base(mysql_interface* db){
		this->db = db;
	}

	inline virtual int deal(void* recvbuf, int rcvlen, char** sendbuf, int* sndlen){
		recv_data_cli_t in(recvbuf);
		cli_proto_head_t head;
		in>>head.len>>head.cmd>>head.id>>head.seq>>head.ret;
		PRI_STRU p_pri_stru;
		if((p_pri_stru = this->cmd_map.get_cmd_fun(head.cmd))!=NULL){
			DEBUG_LOG("I:%04X:%d", head.cmd, head.id);
	
			this->ret=9999;
			//调用相关DB处理函数
			this->ret=(((Croute_func*)this)->p_pri_stru)(recvbuf, sendbuf, sndlen );	
			//提交数据
			if (mysql_commit(&(this->db->handle))!=DB_SUCC){
				this->db->show_error_log("db:COMMIT:err");
			}
	
			return this->ret;
		}else{
			DEBUG_LOG("DB CMD no define  cmdid[%04X]",head.cmd );
			return  CMDID_NODEFINE_ERR;
		}
	}
	virtual ~Cfunc_route_base(){}
};

