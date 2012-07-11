/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		协议
*********************************************************************/

#pragma once

namespace ice{
	#pragma pack(1)
	/*协议包头格式 */
	struct proto_head_t {
		uint32_t len; /* 协议的长度 */
		uint32_t cmd; /* 协议的命令号 */
		uint32_t id; /* 账号 */
		uint32_t seq;/* 序列号 */
		uint32_t ret; /* S->C, 错误码 */
		uint8_t body[]; /* 包体信息 */
		proto_head_t(){
			this->len = 0;
			this->cmd = 0;
			this->id = 0;
			this->seq = 0;
			this->ret = 0;
		}
	};
	#pragma pack()
}

