/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	���ִ����붨��
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
		//���ݿ����
 DB_ERR   	=						4097,
		//ϵͳ���� һ�����ڴ����
 SYS_ERR   					=		1001,

		//����IDû�ж���
 CMDID_NODEFINE_ERR 			=  		1004,
		//KEY �Ѿ�������
 KEY_EXISTED_ERR 			=		1014,
		//��select update, delete ʱ�����׺Ų�����
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
		E_LIB_ERR_CODE_DBPROXY_NO_FIND_CMD = 8193,//route.xml����δ����CMD
		E_LIB_ERR_CODE_DBPROXY_TIME_OUT = 8194,//��ʱ
		E_LIB_ERR_CODE_DBPROXY_MAX = 12287,
	};

}