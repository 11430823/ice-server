/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		Э��
*********************************************************************/

#pragma once

#pragma pack(1)
/*Э���ͷ��ʽ */
struct proto_head_t {
	uint32_t len; /* Э��ĳ��� */
	uint32_t cmd; /* Э�������� */
	uint32_t id; /* �˺� */
	uint32_t seq;/* ���к� */
	uint32_t ret; /* S->C, ������ */
	uint8_t body[]; /* ������Ϣ */
};
#pragma pack()