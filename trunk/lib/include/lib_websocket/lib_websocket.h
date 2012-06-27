
#include "lib_util.h"

#pragma once

/************************************************************************/
/* 
GET / HTTP/1.1
Upgrade: websocket
Connection: Upgrade
Host: 10.1.5.31:8899
Origin: http://localhost:6552
Sec-WebSocket-Key: 4Z7gJp+9aG+i2ymtV61OFA==
Sec-WebSocket-Version: 13

                                                                     */
/************************************************************************/

/************************************************************************/
/*
47 45 54 20 2f 20 48 54 54 50 2f 31 2e 31 0d 0a 55 70 67 72 61 64 65 3a 20 
77 65 62 73 6f 63 6b 65 74 0d 0a 43 6f 6e 6e 65 63 74 69 6f 6e 3a 20 55 70 
67 72 61 64 65 0d 0a 48 6f 73 74 3a 20 31 30 2e 31 2e 35 2e 33 31 3a 38 38 
39 39 0d 0a 4f 72 69 67 69 6e 3a 20 68 74 74 70 3a 2f 2f 6c 6f 63 61 6c 68 
6f 73 74 3a 36 35 35 32 0d 0a 53 65 63 2d 57 65 62 53 6f 63 6b 65 74 2d 4b 
65 79 3a 20 34 5a 37 67 4a 70 2b 39 61 47 2b 69 32 79 6d 74 56 36 31 4f 46 
41 3d 3d 0d 0a 53 65 63 2d 57 65 62 53 6f 63 6b 65 74 2d 56 65 72 73 69 6f 
6e 3a 20 31 33 0d 0a 0d 0a */
/************************************************************************/

#define USE_ICE_LIB_WEBSOCKET

namespace ice{
	class lib_websocket_t
	{
		PRIVATE_R(uint32_t, websocket_len);
	public:
		const static uint32_t WEB_SOCKET_BUF_MAX_LEN = 2048;
		char* get_websocket_buf(){
			return this->websocket_buf;
		}
	public:
		lib_websocket_t(){}
		virtual ~lib_websocket_t(){}
		/**
		* @brief	处理并返回握手包
		* @param	uint32_t len   必须大于8个字节 (开头的("GET ")"47 45 54 20" + 结束的("两个换行")"0d 0a 0d 0a")
		* @return	uint32_t   0:是握手包
		*/

		int proc(char* buf, uint32_t len);
		/**
		* @brief	判断是否完整包
		* @param	char * buf
		* @param	uint32_t len
		* @return	int 0:是, 1:不是, -1:是包,但是长度错误
		*/
		int is_full_pack(char* buf, uint32_t len);
	protected:
		
	private:
		lib_websocket_t(const lib_websocket_t& cr);
		lib_websocket_t& operator=(const lib_websocket_t& cr);
		char websocket_buf[WEB_SOCKET_BUF_MAX_LEN];
	};
}