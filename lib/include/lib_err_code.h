/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	各种错误码定义
	brief:		todo
*********************************************************************/
#pragma once

namespace ice{
	//////////////////////////////////////////////////////////////////////////
	//db
	//[0x00001000 - 0x00001FFF]
	enum E_LIB_ERR_CODE_DB
	{
		E_LIB_ERR_CODE_DB_MIN = 4096,
		//没有定义
#define NO_DEFINE_ERR   					1000
		//系统出错 一般是内存出错
#define SYS_ERR   							1001

		//数据库出错
#define DB_ERR   							1002

		//NET出错
#define NET_ERR   							1003

		//命令ID没有定义
#define CMDID_NODEFINE_ERR 			  		1004
		//协议长度不符合
#define PROTO_LEN_ERR  			 			1005

		//数值越界
#define VALUE_OUT_OF_RANGE_ERR 	 			1006

		//要设置的flag和原有一致 
#define FLAY_ALREADY_SET_ERR 	 			1007

		//数据不完整
#define VALUE_ISNOT_FULL_ERR 	 			1008

		//枚举越界:不在类型的限定范围.(如某一字段只允许[0,1],却填了2)
#define ENUM_OUT_OF_RANGE_ERR 	 			1009

		//返回报文长度有问题
#define PROTO_RETURN_LEN_ERR 	 			1010

		//数据库中数据有误
#define DB_DATA_ERR   						1011

#define NET_SEND_ERR   						1012
#define NET_RECV_ERR   						1013
		//KEY 已经存在了
#define KEY_EXISTED_ERR 					1014
		//KEY 不存在
#define KEY_NOFIND_ERR 						1015

		//DB不允许更新
#define DB_CANNOT_UPDATE_ERR   				1016

		//网络超时
#define NET_TIMEOUT_ERR 					1017

		//当然命令号每分钟超过最大限制
#define CMD_EXEC_MAX_PER_MINUTE_ERR 		1018

#define VALUE_NOENOUGH_E 					1020
#define VALUE_MAX_E 						1021


		//当然命令的md5_tags检查失败
#define CMD_MD5_TAG_ERR 					1021

		//登入时，检查用户名和密码出错
#define CHECK_PASSWD_ERR					1103

		//在insert，米米号已存在
#define USER_ID_EXISTED_ERR				 	1104
		//在select update, delete 时，米米号不存在
#define USER_ID_NOFIND_ERR				  	1105

#define LIST_ID_EXISTED					 	1109
		//列表中的ID  好友列表，黑名单
#define LIST_ID_NOFIND					  	1110
#define LIST_ID_MAX						 	1115

#define KEY_INEXISTENCE_ERR					1116  
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