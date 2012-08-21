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

//发送给客户端的数据
class send_data_cli_t : public lib_send_data_t<proto_head_t>{
public:
	send_data_cli_t()
		:lib_send_data_t<proto_head_t>(send_data){
	}
	virtual void set_head(const proto_head_t& rhead){//uint32_t cmd, uint32_t seq, uint32_t userid, uint32_t ret = 0){
		const uint32_t all_len = this->write_pos;
		this->write_pos = 0;
		*this<<all_len
			<<rhead.cmd
			<<rhead.id
			<<rhead.seq
			<<rhead.ret;
		this->write_pos = all_len;
	}
protected:
private:
	static const uint32_t PACK_DEFAULT_SIZE = 81920;//去掉包头,大概相等
	char send_data[PACK_DEFAULT_SIZE];
	send_data_cli_t(const send_data_cli_t& cr); // 拷贝构造函数
	send_data_cli_t& operator=( const send_data_cli_t& cr); // 赋值函数
};

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

