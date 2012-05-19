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

#ifndef  CFUNC_ROUTE_BASE_INC
#define  CFUNC_ROUTE_BASE_INC
#include "mysql_iface.h"
#include "Cfunc_route_cmd.h"
/*
 * =====================================================================================
 *        Class:  Cfunc_route_base
 *  Description:  
 * =====================================================================================
 */

class Cfunc_route_base:public Cfunc_route_cmd
{
protected: 
    int ret;/*用于保存操作返回值，只是为了方便 */
	//db 连接
	mysql_interface *db;
public:
	inline Cfunc_route_base(mysql_interface * db ):
		db(db)
	{
		this->db=db;
	}


	inline  virtual int deal(char *recvbuf, int rcvlen, char **sendbuf, int *sndlen )
	{
		PRI_STRU * p_pri_stru;
		uint32_t cmdid=((PROTO_HEADER*)recvbuf)->cmd;
		userid_t userid=((PROTO_HEADER*)recvbuf)->id;
		if((p_pri_stru =this->cmdmaplist.getitem(cmdid))!=NULL){
			DEBUG_LOG("I:%04X:%d", cmdid, userid );
			//检查协议频率
			if (! p_pri_stru->exec_cmd_limit.add_count() ){
				DEBUG_LOG("cmd max err:cmdid %u, max_count:%u ",
					cmdid,p_pri_stru->exec_cmd_limit.limit_max_count_per_min);
			
				return CMD_EXEC_MAX_PER_MINUTE_ERR;
			}
	
	
			//检查报文长度
			if (! p_pri_stru->check_proto_size(rcvlen - PROTO_HEADER_SIZE) ){
				DEBUG_LOG("len err pre [%u] send [%d]",
				uint32_t (p_pri_stru->predefine_len+PROTO_HEADER_SIZE ),rcvlen );
				return PROTO_LEN_ERR;
			}
	
			this->ret=9999;
			//调用相关DB处理函数
			this->ret=(((Croute_func*)this)->*(p_pri_stru->p_deal_fun))(recvbuf, sendbuf, sndlen );	
			//提交数据
			if (mysql_commit(&(this->db->handle))!=DB_SUCC){
				this->db->show_error_log("db:COMMIT:err");
			}
	
			return this->ret;
		}else{
			DEBUG_LOG("DB CMD no define  cmdid[%04X]",cmdid );
			return  CMDID_NODEFINE_ERR;
		}
	}

	virtual ~Cfunc_route_base (){ }
	
}; /* -----  end of class  Cfunc_route_base  ----- */

#endif   /* ----- #ifndef CFUNC_ROUTE_BASE_INC  ----- */
