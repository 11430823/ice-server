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
	//dbproxy 
	//[0x00000001 - 0x00000FFF]
	//[0 - 4095]
	enum E_LIB_ERR_CODE_DBPROXY
	{
		E_LIB_ERR_CODE_DBPROXY_MIN = 1,
		E_LIB_ERR_CODE_DBPROXY_NO_FIND_CMD = 100,//route.xml����δ����CMD
		E_LIB_ERR_CODE_DBPROXY_TIME_OUT = 101,//��ʱ
		E_LIB_ERR_CODE_DBPROXY_MAX = 4095,
	};
	//////////////////////////////////////////////////////////////////////////
	//db
	//[0x00001000 - 0x00001FFF]
	enum E_LIB_ERR_CODE_DB
	{

	};

}