#include "CtableRoute100.h"

char * CtableRoute100::get_table_name(uint32_t id)
{
	this->id=id;
	this->db->select_db(this->db_name);

	sprintf (this->db_table_name,"%s.%s_%02d",
	this->db_name_pre,this->table_name_pre,id%100);
	return this->db_table_name;
}