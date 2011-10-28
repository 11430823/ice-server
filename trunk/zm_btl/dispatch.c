#include "dispatch.h"
#include "proto.h"
#include "log.h"
#include "cmd.h"
#include "data.h"

int dispatch_client(void *data, fdsession_t *fdsess)
{
	cli_proto_head_t* phead = (cli_proto_head_t*)data;
	KDEBUG_LOG(phead->id, "[cmd:%u, len:%u, ret:%u, seq_num:%u]", phead->cmd, phead->len, phead->ret, phead->seq_num);

	if (!g_cmd.is_able(phead->cmd)){
		KERROR_LOG(phead->id,"cmd unable [cmd:%u]", phead->id);
		return 0;
	}
	g_cmd.handle_client_fns[phead->cmd](CMD_HANDLE_CLIENT_RAG);
	return 0;
}