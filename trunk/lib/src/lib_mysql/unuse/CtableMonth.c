#include "CtableMonth.h"


CtableMonth::CtableMonth(mysql_interface * db,const char * db_name_pre,  
	const	char * table_name_pre,const char* id_name ) :CtableRoute(
	db,db_name_pre,table_name_pre)
{ 

}

char*  CtableMonth::get_table_name()
{
	return get_table_name( get_year_month(time(NULL)) );
}

char * CtableMonth::get_table_name(uint32_t id)
{
	if (id>100000000)//ÊÇ½»Ò×ºÅ
	{
		uint32_t months=id/100000000-1;
		id=(2008+months/12)*100+months%12+1;
	}
	this->db->select_db(this->db_name);

	sprintf (this->db_table_name,"%s.%s_%6.6d",
	this->db_name_pre, this->table_name_pre,id);
	return this->db_table_name;
}
