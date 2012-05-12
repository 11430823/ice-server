#include "CtableDate_100.h"

CtableDate_100::CtableDate_100(mysql_interface * db,const char * db_name_pre,  
	const	char * table_name_pre,const char* id_name ) :CtableRoute(
	db,db_name_pre,table_name_pre)
{ 

}

char*  CtableDate_100::get_table_name(uint32_t id, time_t logtime )
{
	this->id=id;
	this->db->select_db(this->db_name);


	sprintf (this->db_table_name,"%s.%s_%02d_%8.8d",
		this->db_name_pre,  this->table_name_pre,id%100,get_date(logtime) );
	return this->db_table_name;
}

