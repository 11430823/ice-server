#include "CtableRoute10.h"

char * CtableRoute10::get_table_name(uint32_t id)
{
	this->id = id;
	this->db->select_db(this->db_name_pre);

	sprintf (this->db_table_name, "%s.%s_%01d",
	this->db_name_pre, this->table_name_pre, id%10);
	return this->db_table_name;
}