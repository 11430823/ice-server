#include <fcntl.h>

#include "lib_tcp.h"

int ice::lib_tcp::set_io_block( int fd, bool is_block )
{
	int val;
	if (is_block) {
		val = (~O_NONBLOCK & fcntl(fd, F_GETFL));
	} else {
		val = (O_NONBLOCK | fcntl(fd, F_GETFL));
	}
	return fcntl(fd, F_SETFL, val);
}