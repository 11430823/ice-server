#include "CtableRoute.h"

CtableRoute::CtableRoute(mysql_interface* db,const  char* db_name_pre, const char* table_name_pre) 
	:Ctable(db)	
{ 
	::strncpy(this->db_name_pre, db_name_pre, sizeof(this->db_name_pre));
	::strncpy(this->table_name_pre,table_name_pre, sizeof(this->table_name_pre ) );
}

char* CtableRoute::get_table_name(uint32_t id)
{
	this->id = id;
	::sprintf(this->db_name,"%s_%02d", this->db_name_pre, id%100);
	this->db->select_db(this->db_name);

	::sprintf(this->db_table_name,"%s_%02d.%s_%02d",
	this->db_name_pre,id%100, this->table_name_pre,(id/100)%100);
	return this->db_table_name;
}