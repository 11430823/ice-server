/*
 * =====================================================================================
 * 
 *       Filename:  func_route_base.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月02日 16时40分57秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CFUNC_ROUTE_DB_V2_INC
#define  CFUNC_ROUTE_DB_V2_INC
#include "mysql_iface.h"
#include "Cfunc_route_cmd_v2.h"
#include <map>
#include <libtaomee++/utils/tcpip.h>
/*
 * =====================================================================================
 *        Class:  Cfunc_route_db_v2
 *  Description:  
 * =====================================================================================
 */
struct stru_db_name {
	char 	db_name[128];
};
struct stru_cmd_db_name {
	uint16_t cmdid;
	stru_db_name db;
};

class Cfunc_route_db_v2:public Cfunc_route_cmd_v2
{
protected: 
    bool sync_user_data_flag;/*当前是否处于同步数据状态 */
    int ret;/*用于保存操作返回值，只是为了方便 */

	//db 连接
	mysql_interface *db;
	//命令所操作的db设置 
	std::map<uint32_t, stru_db_name > cmd_db_map;
public:
	inline Cfunc_route_db_v2(mysql_interface * db ):
		db(db)
	{
		this->db=db;
		this->sync_user_data_flag=config_get_intval("SYNC_USER_DATA_FLAG",0);
	}

	//设置命令对应的db名字前缀
	inline void init_cmd_db_map(stru_cmd_db_name *p_stru_cmd_db,uint32_t size  ){
		for (uint32_t i=0;i<size;i++ ){
			 this->cmd_db_map.insert(std::pair<uint32_t, stru_db_name >  (
					 GET_ROUTE( (p_stru_cmd_db+i)->cmdid) , 
					 	(p_stru_cmd_db+i)->db
					 ));
		}
	}


	virtual int deal_func(uint16_t cmdid, userid_t userid, Cmessage * c_in, Cmessage * c_out ,P_DEALFUN_T p_func ,bool is_commit )
	{
		//设置当前命令要处理db
		uint32_t route_db_id=GET_ROUTE(cmdid);

		std::map<uint32_t, stru_db_name>::iterator cmd_db_it; 
		cmd_db_it=this->cmd_db_map.find(route_db_id);
	
		//处理在线分裂数据通过
		if (this->sync_user_data_flag ){//是否打开同步标志
			ret=this->do_sync_data(userid,cmdid);
			if (ret!=SUCC) return ret;
		}
					

		//调用相关DB处理函数
		this->ret=9999;
		this->ret=(((Croute_func*)this)->*(p_func))(
				userid,c_in ,c_out);

		if (is_commit){
			//提交数据
			if (mysql_commit(&(this->db->handle))!=DB_SUCC){
				this->db->show_error_log("db:COMMIT:err");
			}
		}
		return this->ret;
	}

	virtual int do_rollback( )
	{
		if (mysql_rollback(&(this->db->handle))!=DB_SUCC){
			this->db->show_error_log("db:ROLLBACK :err");
		}
		return 0;
	}



	virtual int  do_sync_data(uint32_t userid, uint16_t cmdid) { return SUCC; }
	virtual ~Cfunc_route_db_v2 (){ }
	
}; /* -----  end of class  Cfunc_route_db_v2  ----- */

#endif   /* ----- #ifndef CFUNC_ROUTE_DB_V2_INC  ----- */

