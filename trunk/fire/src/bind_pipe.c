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

	// ��������ΪFD_CLOEXEC��ʾ������ִ��exec����ʱ��fd����ϵͳ�Զ��ر�,��ʾ�����ݸ�exec�������½���
	::fcntl (this->handles[E_PIPE_INDEX_RDONLY], F_SETFD, FD_CLOEXEC);
	::fcntl (this->handles[E_PIPE_INDEX_WRONLY], F_SETFD, FD_CLOEXEC);
	return 0;
}