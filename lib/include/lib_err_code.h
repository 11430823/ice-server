/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	各种错误码定义
	brief:		ok
*********************************************************************/
#pragma once

namespace ice{
	//////////////////////////////////////////////////////////////////////////
	//db
	//[0x00001000 - 0x00001FFF]
	enum E_LIB_ERR_CODE_DB
	{
		E_LIB_ERR_CODE_DB_MIN = 4096,
		//数据库出错
 DB_ERR   	=						4097,
		//系统出错 一般是内存出错
 SYS_ERR   					=		1001,

		//命令ID没有定义
 CMDID_NODEFINE_ERR 			=  		1004,
		//KEY 已经存在了
 KEY_EXISTED_ERR 			=		1014,
		//在select update, delete 时，米米号不存在
 USER_ID_NOFIND_ERR			=	  	1105,
 KEY_INEXISTENCE_ERR			=		1116 , 
		E_LIB_ERR_CODE_DB_MAX = 8191,
	};
	//////////////////////////////////////////////////////////////////////////
	//dbproxy 
	//[0x00002000 - 0x00002FFF]
	//[8192 - 12287]
	enum E_LIB_ERR_CODE_DBPROXY
	{
		E_LIB_ERR_CODE_DBPROXY_MIN = 8192,
		E_LIB_ERR_CODE_DBPROXY_NO_FIND_CMD = 8193,//route.xml表中未定义CMD
		E_LIB_ERR_CODE_DBPROXY_TIME_OUT = 8194,//超时
		E_LIB_ERR_CODE_DBPROXY_MAX = 12287,
	};

}