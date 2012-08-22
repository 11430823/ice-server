#include <lib_log.h>
#include <lib_proto/lib_proto.h>
#include <lib_proto/lib_msg.h>

#include "../../protocol/cpp/protocol_online.h"
#include "handle_cli.h"

handle_cli g_handle_cli;

int handle_cli::handle( const void* pkg, int pkglen )
{
	ice::proto_head_t* head = (ice::proto_head_t*)pkg;
	TRACE_LOG("[len:%u, cmd:%u, seq:%u, ret:%u, uid:%u, pkglen:%d]",
		head->len, head->cmd, head->seq, head->ret, head->id, pkglen);
	
	ice::lib_cmd_t<P_DEALFUN_T>* p_cmd_item = cmd_map.get(head->cmd);
	if (NULL == p_cmd_item){
		ERROR_LOG("cli handle empty [cmd=%u]", head->cmd);
		return -1;
	} else {
		//还原对象
		p_cmd_item->p_msg->init();
		ice::lib_msg_byte_t msg_byte((char*)pkg + sizeof(ice::proto_head_t), pkglen - sizeof(ice::proto_head_t));
		if (!p_cmd_item->p_msg->read(msg_byte)){
			ERROR_LOG("还原对象失败 [cmd=%u]", head->cmd);
			return -1;
		} else if (!msg_byte.is_end()){
			ERROR_LOG("还原对象失败,长度不符 [cmd=%u]", head->cmd);
			return -1;
		} else {
			P_DEALFUN_T p_func = p_cmd_item->func;
			if (NULL != p_func){
				return (this->*p_func)(p_cmd_item->p_msg);
			}
		}
	}
	return 0;
}

handle_cli::handle_cli()
{
	//初始化CMD和处理函数
#undef  BIND_PROTO_CMD
#define BIND_PROTO_CMD(cmd_id, fun_name, p_in)\
	this->cmd_map.insert(cmd_id, &handle_cli::fun_name, new (p_in));
#include "../../protocol/cpp/protocol_online_cmd.h"
#undef  BIND_PROTO_CMD
}

int handle_cli::cli_walk(DEAL_FUN_ARG)
{
	cli_walk_in* p_in = P_IN;
	DEBUG_LOG("cli_walk [x:%u, y:%u, size:%lu]", p_in->pos.x, p_in->pos.y, p_in->id.size());
	FOREACH(p_in->id, it){
		uint32_t id = *it;
		DEBUG_LOG("[id:%u]", id);
	}
	return 0;
}
