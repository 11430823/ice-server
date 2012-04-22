#include "lib_include.h"
#include "lib_file.h"
#include "lib_net_util.h"
#include "lib_tcp.h"

int ice::lib_tcp_t::set_reuse_addr( int s )
{
	const int flag = 1;
	return setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
}

int ice::lib_tcp_t::send( const void* buf, int total )
{
	int send_bytes = 0;
	for (int cur_len = 0; send_bytes < total; send_bytes += cur_len) {
		//MSG_NOSIGNAL: linux man send ²é¿´
		cur_len = ::send(this->fd, (char*)buf + send_bytes, total - send_bytes, MSG_NOSIGNAL);
		if (-1 == cur_len) {
			if (EINTR == errno) {
				cur_len = 0;
			} else if (EAGAIN == errno || EWOULDBLOCK == errno) {
				break;
			} else {
				return -1;
			}
		}
	}
	return send_bytes;
}

int ice::lib_tcp_t::recv( void* buf, int bufsize )
{
	return HANDLE_EINTR(::recv(this->fd, buf, bufsize, 0));
}

const char* ice::lib_tcp_t::get_ip_str()
{
	return lib_net_t::ip2str(this->ip);
}
