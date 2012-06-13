#include "func_rount.h"

int Cfunc_route::tool_add(DEAL_FUN_ARG)
{
	tool_add_in_t info;
	in>>info.id>>info.num;
	this->ret = this->tool.add(head.id, info.id, info.num);

	return this->ret;
}

int Cfunc_route::tool_reduce(DEAL_FUN_ARG)
{
	tool_reduce_in_t info;
	in>>info.id>>info.num;
	this->ret = this->tool.reduce(head.id, info.id, info.num);

	return this->ret;
}

int Cfunc_route::tool_get_all(DEAL_FUN_ARG)
{
	this->tool.get_all(head.id, this->send_data);
	this->send_data.set_head(head);
	*sendbuf = this->send_data.data();
	sndlen =  this->send_data.len();
	return 0;
}

int Cfunc_route::user_add(DEAL_FUN_ARG)
{
	user_add_in_t info;
	in.unpack_str(info.nick, USER_NICK_LEN);
	return this->user.add(head.id, info.nick);
}

int Cfunc_route::user_get_all(DEAL_FUN_ARG)
{
	return this->user.get_all(head.id, this->send_data);
}
