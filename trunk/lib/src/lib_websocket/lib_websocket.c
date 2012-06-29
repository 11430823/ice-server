
#include <string.h>

#include "lib_log.h"

#include "lib_websocket/lib_websocket.h"

int ice::lib_websocket_t::is_full_pack( char* buf, uint32_t len )
{
	if (len < 8){
		return 1;
	}
	static const char begin_flag[4] = {'G', 'E', 'T', ' '};
	static char end_flag[4] = {'\r', '\n', '\r', '\n'};

	//判断是否请求握手包
	if (0 != memcmp((void*)begin_flag, (void*)buf, 4)){
		return 1;
	}

	//判断是否整个包完整
	if (len > WEB_SOCKET_BUF_MAX_LEN){
		return -1;
	}
	uint32_t end_idx = len - 4;
	char* tmp = buf + end_idx;
	if (0 != memcmp((void*)end_flag, (void*)tmp, 4)){
		return 1;
	}

	std::string str_hex;
	bin2hex(str_hex, buf, len);
	INFO_LOG("websocket pack[%s]", str_hex.c_str());
	return 0;
}

int ice::lib_websocket_t::proc( char* buf, uint32_t len )
{
	if (0 != this->is_full_pack(buf, len)){
		return 1;
	}
	/*
	c->s
	GET /chat HTTP/1.1
	Upgrade: websocket
	Connection: Upgrade
	Host: www.zendstudio.net:9108
	Origin: http://www.zendstudio.net
	Sec-WebSocket-Key: U00QUfV1CRfIIU0NkcUCnA==
	Sec-WebSocket-Version: 13
	Sec-WebSocket-Extensions: x-webkit-deflate-frame

	s->c
	HTTP/1.1 101 Switching Protocols
	Upgrade: websocket
	Connection: Upgrade
	Sec-WebSocket-Accept: 7GGzyIJjf9bX7pej+3tc5Vv87S0=
	WebSocket-Origin: http://www.zendstudio.net
	WebSocket-Location: ws://www.zendstudio.net:9108/chat
	*/

	//todo 组包
	static const std::string str_tag = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
	std::vector<std::string> ret;
	std::string str_src;
	str_src.assign(buf, len);
	std::string str_separator = '\r';
	split(str_src, 

	return 0;
}
