#include <lib_log.h>
#include <lib_proto/lib_proto.h>

#include "handle_cli.h"

handle_cli g_handle_cli;

int handle_cli::handle( const void* pkg, int pkglen )
{
	ice::proto_head_t* head = (ice::proto_head_t*)pkg;
	TRACE_LOG("[len:%u, cmd:%u, seq:%u, ret:%u, uid:%u, pkglen:%d]",
		head->len, head->cmd, head->seq, head->ret, head->id, pkglen);


	return 0;
}
