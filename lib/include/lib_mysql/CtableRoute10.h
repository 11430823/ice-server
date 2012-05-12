/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		±íÂ·ÓÉ1x10
*********************************************************************/

#pragma once

#include "CtableRoute.h"

class CtableRoute10 : public CtableRoute
{
protected:
	virtual char*  get_table_name(uint32_t id);
public:
	CtableRoute10(mysql_interface* db, const char* db_name_pre, const char* table_name_pre)
		: CtableRoute(db, db_name_pre, table_name_pre){}
	virtual ~CtableRoute10(void){} 
};


