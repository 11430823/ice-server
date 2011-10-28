/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	协议
	brief:		
*********************************************************************/

#pragma once

#pragma pack(1)

/* SERVER和CLIENT的协议包头格式 */
struct cli_proto_head_t {
	uint32_t len; /* 协议的长度 */
	uint16_t cmd; /* 协议的命令号 */
	uint32_t id; /* 账号 */
	uint32_t seq_num;/* 序列号 */
	uint32_t ret; /* S->C, 错误码 */
	uint8_t body[]; /* 包体信息 */
};

#pragma pack()