/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	Э��
	brief:		
*********************************************************************/

#pragma once

#pragma pack(1)

/* SERVER��CLIENT��Э���ͷ��ʽ */
struct cli_proto_head_t {
	uint32_t len; /* Э��ĳ��� */
	uint16_t cmd; /* Э�������� */
	uint32_t id; /* �˺� */
	uint32_t seq_num;/* ���к� */
	uint32_t ret; /* S->C, ������ */
	uint8_t body[]; /* ������Ϣ */
};

#pragma pack()