
#pragma once

#include <mysql/mysql.h>

#include <lib_log.h>

#include "db_error_base.h" 
#include "proto_header.h" 

#define mysql_str_len(n) ((n) * 2 + 1)

#define set_mysql_string(dst, src, n) \
	mysql_real_escape_string(&(this->db->handle), dst, src, n)

inline uint64_t atoi_safe(char* str) 
{
	return (str!= NULL ? atoll(str) : 0);
}

//依次得到row[i]
// 在STD_QUERY_WHILE_BEGIN  和 STD_QUERY_ONE_BEGIN
#define NEXT_FIELD 	 (row[++_fi])

#define GET_NEXT_FIELD_INT(out, db_type)\
	(out)<<(db_type)atoi_safe(NEXT_FIELD)

#define GET_NEXT_FIELD_BIN(out, max_len)\
	{\
		++_fi;\
		mysql_fetch_lengths(res);\
		uint32_t real_len = res->lengths[_fi] < max_len ? res->lengths[_fi] : max_len;\
		out<<real_len;\
		out.pack_str(row[_fi], real_len);\
	}

//得到int
#define INT_CPY_NEXT_FIELD(value)  (value) = atoi_safe(NEXT_FIELD)

//多条记录
#define STD_QUERY_WHILE_BEGIN(sqlstr, count) \
	{\
		MYSQL_RES* res;\
		MYSQL_ROW  row;\
		this->db->set_id(this->id);\
		this->ret = this->db->exec_query_sql(sqlstr, &res);\
		if (0 == this->ret){\
			count = mysql_num_rows(res);\
			while((row = mysql_fetch_row(res))){\
				int _fi = -1;

#define STD_QUERY_WHILE_END() \
			}\
			mysql_free_result(res);	\
		}else {\
			this->ret = DB_ERR;\
		}\
	}

#define GET_ROUTE(cmd_id) ((cmd_id&0x8000)?(70+((cmd_id&0x7E00)>>6)+((cmd_id &0x00E0)>>5)):(cmd_id>>9))

inline double atof_safe (char *str) 
{
	return 	(str!= NULL ? atof(str):0 );
}

//正常的db 连接 
#define STD_OPEN_AUTOCOMMIT()	   mysql_autocommit(&(this->db->handle), T)
#define STD_ROLLBACK()  mysql_rollback(&(this->db->handle))
#define STD_CLOSE_AUTOCOMMIT(nret) {\
		if (mysql_autocommit(&(this->db->handle), 0)==SUCC){\
				nret = SUCC;\
		}else {\
				nret = DB_ERR;\
		}\
	}
#define STD_COMMIT(nret) {\
		if (mysql_commit(&(this->db->handle))==SUCC){\
				nret = SUCC;\
		}else {\
				nret = DB_ERR;\
		}\
	}

//变长方式copy
#define BIN_CPY_NEXT_FIELD( dst,max_len)  ++_fi; \
		mysql_fetch_lengths(res); \
		res->lengths[_fi]<max_len? \
		memcpy(dst,row[_fi],res->lengths[_fi] ): memcpy(dst,row[_fi],max_len)



//得到double的值
#define DOUBLE_CPY_NEXT_FIELD(value )  (value)=atof_safe(NEXT_FIELD)



//----------------------列表模式-----------------------------
//如果
#ifdef DB_GET_LIST_NO_MALLOC   /* 没有malloc */
// use dbser_return_buf for list ,set record count to count
#define STD_QUERY_WHILE_BEGIN( sqlstr,pp_list,p_count )  \
    {   MYSQL_RES *res;\
        MYSQL_ROW  row;\
        int list_size;\
        int i;\
        this->db->set_id(this->id);\
        if (( this->db->exec_query_sql(sqlstr,&res))==SUCC){\
            *p_count=mysql_num_rows(res);\
            *pp_list =(typeof(*pp_list))this->dbser_return_buf;\
            list_size = sizeof(typeof(**pp_list))*(*p_count);\
            if(list_size > PROTO_MAX_SIZE ){\
                DEBUG_LOG("THE SYS_ERR");\
                return SYS_ERR;\
            }\
            memset(*pp_list,0,PROTO_MAX_SIZE );\
            i=0;\
            while((row = mysql_fetch_row(res))){\
                int _fi;\
                _fi=-1;

#define STD_QUERY_WHILE_END()  \
                i++;\
            }\
            mysql_free_result(res); \
            return SUCC;\
        }else {\
            return DB_ERR;\
        }\
    }

#elif defined DB_GET_LIST_NEW

#define STD_QUERY_WHILE_BEGIN( sqlstr,item_list )  \
	{ \
		typeof(item_list)&_item_list=item_list;\
		MYSQL_RES *res;\
		MYSQL_ROW  row;\
        this->db->set_id(this->id);\
		if (( this->db->exec_query_sql(sqlstr,&res))==SUCC){\
			typeof(item_list[0]) item ;\
			while((row = mysql_fetch_row(res))){\
				int _fi;\
			   	_fi=-1;
	

#define STD_QUERY_WHILE_END()  \
			_item_list.push_back(item);\
			}\
			mysql_free_result(res);	\
			return SUCC;\
		}else {\
			return DB_ERR;\
		}\
	}

#else 




#endif

#define STD_QUERY_WHILE_BEGIN_NEW( sqlstr,item_list ) \
	{ \
		typeof(item_list)&_item_list=item_list;\
		MYSQL_RES *res;\
		MYSQL_ROW  row;\
        this->db->set_id(this->id);\
		if (( this->db->exec_query_sql(sqlstr,&res))==SUCC){\
			typeof(item_list[0]) item ;\
			while((row = mysql_fetch_row(res))){\
				int _fi;\
			   	_fi=-1;
	
#define STD_QUERY_WHILE_END_NEW()  \
            _item_list.push_back(item);\
            }\
            mysql_free_result(res); \
            return SUCC;\
        }else {\
            return DB_ERR;\
        }\
    }


#define STD_QUERY_ONE_BEGIN(sqlstr, no_find_err) {\
		uint32_t ret;\
		MYSQL_RES* res;\
		MYSQL_ROW row;\
		int rowcount;\
        this->db->set_id(this->id);\
		ret = this->db->exec_query_sql(sqlstr, &res);\
		if (0 == ret){\
			rowcount = mysql_num_rows(res);\
			if (1 != rowcount) {\
	 			mysql_free_result(res);\
				DEBUG_LOG("no select a record [no_find_err:%u]", no_find_err);\
				return no_find_err;\
			}else {\
				row = mysql_fetch_row(res);\
				int _fi;\
				_fi = -1;

#define STD_QUERY_ONE_END()\
				mysql_free_result(res);\
				return 0;\
			}\
		}else {\
			return DB_ERR;\
		}\
	}

#define STD_QUERY_ONE_END_WITHOUT_RETURN()  \
				mysql_free_result(res);		 \
			} \
		}else { \
			return DB_ERR;	 \
		}\
	}



#include <time.h>
inline  int  mysql_date( char*date_str, time_t t , int len  )
{
	  return strftime(date_str ,len, "%Y/%m/%d %H:%M:%S", localtime(&t));
}
inline int get_date(time_t t  ) 
{
	struct tm tm_tmp;
	localtime_r(&t, &tm_tmp) ;
	return (tm_tmp.tm_year+1900)*10000+(tm_tmp.tm_mon+1)*100+tm_tmp.tm_mday;
}

inline int get_year_month(time_t t  ) 
{
	struct tm tm_tmp;
	localtime_r(&t, &tm_tmp) ;
	return (tm_tmp.tm_year+1900)*100+tm_tmp.tm_mon+1;
}


inline uint32_t hash_str(const char * key )
{
	register unsigned int h;
	register unsigned char *p; 
	for(h=0, p = (unsigned char *)key; *p ; p++)
		h = 31 * h + *p; 
	return h;
}

inline char * set_space_end(char * src, int len  )
{
    int i=0;
    while (i<len &&  src[i] !='\0' ) i++;

    for (;i<len;i++) src[i]=' ';
    return src ;

}