#include "CtableRoute100x10.h"

char * CtableRoute100x10::get_table_name(uint32_t id)
{
	this->id=id;
	sprintf (this->db_name,"%s_%02d", this->db_name_pre, id%100 );
	this->db->select_db(this->db_name);

	sprintf (this->db_table_name,"%s_%02d.%s_%01d",
	this->db_name_pre, id%100, this->table_name_pre, (id%10000)/1000);
	return this->db_table_name;
}