/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		等待db的数据
*********************************************************************/

#pragma once

#pragma pack(1)
struct wait_db_t 
{
	uint32_t key;
	uint32_t seq;
};
#pragma pack()