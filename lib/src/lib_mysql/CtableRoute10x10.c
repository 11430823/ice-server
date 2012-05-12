#include "CtableRoute10x10.h"

char* CtableRoute10x10::get_table_name(uint32_t id)
{
	this->id=id;
	::sprintf (this->db_name,"%s_%01d", this->db_name_pre,id%10 );
	this->db->select_db(this->db_name);

	::sprintf (this->db_table_name,"%s_%01d.%s_%01d",
	this->db_name_pre,id%10, this->table_name_pre,(id/10)%10);
	return this->db_table_name;
}