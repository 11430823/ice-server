#include <string.h>

#include "cmd.h"
#include "log.h"


int Cmd::init_handle()
{
	memset(handle_client_fns, 0, sizeof(handle_client_fns));
	handle_client_fns[e_login_cmd] = handle_login;
	return 0;
}

//////////////////////////////////////////////////////////////////////////
//处理消息
int Cmd::handle_login(CMD_HANDLE_CLIENT_RAG)
{
	KDEBUG_LOG(0, "login");
	return 0;
}