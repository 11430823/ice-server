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
		e_lib_err_code_dbproxy_min = 1,
		e_lib_err_code_dbproxy_no_find_cmd = 100,//route.xml����δ����CMD
		e_lib_err_code_dbproxy_time_out = 101,//��ʱ
		e_lib_err_code_dbproxy_max = 4095,
	};
	//////////////////////////////////////////////////////////////////////////
	//db
	//[0x00001000 - 0x00001FFF]
	enum E_LIB_ERR_CODE_DB
	{

	};

}