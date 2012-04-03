#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <lib_log.h>

#include "bind_pipe.h"

bind_pipe_t::bind_pipe_t()
{
	::memset(this->handles, 0, sizeof(this->handles));
}

int bind_pipe_t::create()
{
	if (-1 == ::pipe (this->handles)){
		BOOT_LOG(-1, "PIPE CREATE FAILED [err:%s]", strerror(errno));
	}

	::fcntl (this->handles[E_PIPE_INDEX_RDONLY], F_SETFL, O_NONBLOCK | O_RDONLY);
	::fcntl (this->handles[E_PIPE_INDEX_WRONLY], F_SETFL, O_NONBLOCK | O_WRONLY);

	// 这里设置为FD_CLOEXEC表示当程序执行exec函数时本fd将被系统自动关闭,表示不传递给exec创建的新进程
	::fcntl (this->handles[E_PIPE_INDEX_RDONLY], F_SETFD, FD_CLOEXEC);
	::fcntl (this->handles[E_PIPE_INDEX_WRONLY], F_SETFD, FD_CLOEXEC);
	return 0;
}