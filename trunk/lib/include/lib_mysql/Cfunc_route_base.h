/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		
*********************************************************************/

#pragma once

#include <lib_log.h>
#include <lib_util.h>

#include "mysql_iface.h"
#include "Cfunc_route_cmd.h"
#include "proto_header.h"

class Cfunc_route;

class Cfunc_route_base
{
	PROTECTED_R(mysql_interface*, db);//db 连接
	PROTECTED_R(int, ret);/*用于保存操作返回值，只是为了方便 */
	PROTECTED_R(Ccmdmap, cmd_map);
	PROTECTED_R_REF(ice::lib_send_data_cli_t, send_data);

public:
	Cfunc_route_base(mysql_interface* db){
		this->db = db;
		this->ret = 0;
		this->send_data.init();
	}

	inline virtual int deal(DEAL_FUN_ARG){
		this->send_data.init();

		P_DEALFUN_T p_pri_stru = this->cmd_map.get_cmd_fun(head.cmd);
		if(NULL != p_pri_stru){
			DEBUG_LOG("I[cmd:%08X, id:%u, len:%u, seq=%u, ret=%u]",
				head.cmd, head.id, head.len, head.seq, head.ret);

			this->ret=9999;
			//调用相关DB处理函数
			this->ret = (((Cfunc_route*)this)->*p_pri_stru)(head, in, sendbuf, sndlen);	
			//提交数据
			if (0 != mysql_commit(&(this->db->handle))){
				this->db->show_error_log("db mysql_commit err!!!");
			}
			return this->ret;
		}else{
			DEBUG_LOG("db cmd no define [cmd:%08X]", head.cmd);
			return  CMDID_NODEFINE_ERR;
		}
 		return 0;
	}
	virtual ~Cfunc_route_base(){}
};

