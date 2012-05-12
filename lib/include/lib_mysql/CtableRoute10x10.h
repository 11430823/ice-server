/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		±íÂ·ÓÉ10x10
*********************************************************************/

#include "CtableRoute.h"

class CtableRoute10x10 : public CtableRoute 
{
protected:
	virtual char* get_table_name(uint32_t id);
public:
	CtableRoute10x10(mysql_interface* db, const char* db_name_pre, const char* table_name_pre)
		: CtableRoute(db, db_name_pre, table_name_pre){}
	virtual ~CtableRoute10x10(void){}; 
};