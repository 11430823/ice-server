
#include <string.h>

#include "lib_log.h"

#include "lib_websocket/lib_websocket.h"
//当dst传入null时将会使用 static buf,最大长度是500
inline char * bin2hex(char* dst, char* src, int len, int max_len=0xFFFF)
{
	static char buf[500*3+1];
	if (dst==NULL){
		max_len=500;
		dst=buf;
	}

	int hex;
	int i;
	int di;
	if (len>max_len) len=max_len;
	for(i=0;i<len;i++){
		hex=((unsigned char)src[i])>>4;
		di=i*3;
		dst[di]=hex<10?'0'+hex:'A'-10 +hex ;
		hex=((unsigned char)src[i])&0x0F;
		dst[di+1]=hex<10?'0'+hex:'A'-10 +hex ;
		dst[di+2]=' ';
	}
	dst[len*3]=0;
	return dst;
}

int ice::lib_websocket_t::is_full_pack( char* buf, uint32_t len )
{
	char outbuf[13000];
	bin2hex(outbuf, buf, len, 1000);
	TRACE_LOG("CI[%s]", outbuf);
	

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
	return 0;
}

int ice::lib_websocket_t::proc( char* buf, uint32_t len )
{
	if (0 != this->is_full_pack(buf, len)){
		return 1;
	}
	//todo 组包

	return 0;
}
