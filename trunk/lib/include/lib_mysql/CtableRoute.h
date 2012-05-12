/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		表路由(默认100x100)
*********************************************************************/

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#include "Ctable.h"

const uint32_t DB_NAME_PRE_MAX_LEN = 40;
const uint32_t TABLE_PRE_NAME_MAX_LEN = 40;

class CtableRoute : public Ctable 
{
protected:
	char table_name_pre[TABLE_PRE_NAME_MAX_LEN];
	char db_name_pre[DB_NAME_PRE_MAX_LEN];
	/**
	 * @brief	获取DB.TABLE形式的表名
	 */
	virtual char* get_table_name(uint32_t id);
public:
	/**
	 * @brief	表名的前面部分：如： t_user_pet_00 --  t_user_pet_99, 写入的是t_user_pet
	 */
	CtableRoute(mysql_interface* db, const char* db_name_pre, const char* table_name_pre);
	virtual ~CtableRoute(void){} 
};