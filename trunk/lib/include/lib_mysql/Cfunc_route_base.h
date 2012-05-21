/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		
*********************************************************************/

#pragma once

#include <lib_util.h>

#include "mysql_iface.h"
#include "Cfunc_route_cmd.h"
#include "proto_header.h"

class Cfunc_route_base
{
	PROTECTED_R(mysql_interface*, db);//db 连接
	PROTECTED_R(int, ret);/*用于保存操作返回值，只是为了方便 */
	PROTECTED_R(Ccmdmap, cmd_map);
	PROTECTED_R_REF(send_data_cli_t, send_data);
public:
	Cfunc_route_base(mysql_interface* db){
		this->db = db;
		this->ret = 0;
		this->send_data.init();
	}

	inline virtual int deal(const void* recvbuf, int rcvlen, char** sendbuf, int* sndlen){
		this->send_data.init();
		recv_data_cli_t in(recvbuf);
		cli_proto_head_t head;
		in>>head.len>>head.cmd>>head.id>>head.seq>>head.ret;
		PRI_STRU p_pri_stru;
		if((p_pri_stru = this->cmd_map.get_cmd_fun(head.cmd))!=NULL){
			DEBUG_LOG("I[cmd:%08X, id:%u]", head.cmd, head.id);
	
			this->ret=9999;
			//调用相关DB处理函数
			this->ret = (p_pri_stru)(head, in, sendbuf, sndlen);	
			//提交数据
			if (0 != mysql_commit(&(this->db->handle))){
				this->db->show_error_log("db mysql_commit err!!!");
			}
			return this->ret;
		}else{
			DEBUG_LOG("db cmd no define [cmd:%08X]", head.cmd);
			return  CMDID_NODEFINE_ERR;
		}
	}
	virtual ~Cfunc_route_base(){}
};

