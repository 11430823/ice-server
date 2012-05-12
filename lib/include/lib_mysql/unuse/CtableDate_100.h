#ifndef  CtableDate_100_INCL
#define  CtableDate_100_INCL
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "proto_header.h"
#include "CtableRoute.h"


class CtableDate_100 : public CtableRoute {
	private:
	public:
		char*  get_table_name(uint32_t id, time_t logtime );
	protected:
		CtableDate_100(mysql_interface * db,const char * db_name_pre,  
			const char * table_name_pre,const  char* id_name ) ;

		virtual ~CtableDate_100(void){} 
};

#endif   /* ----- #ifndef CtableDate_100_INCL  ----- */

