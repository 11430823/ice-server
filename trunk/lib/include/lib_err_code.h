/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	���ִ����붨��
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
		//û�ж���
#define NO_DEFINE_ERR   					1000
		//ϵͳ���� һ�����ڴ����
#define SYS_ERR   							1001

		//���ݿ����
#define DB_ERR   							1002

		//NET����
#define NET_ERR   							1003

		//����IDû�ж���
#define CMDID_NODEFINE_ERR 			  		1004
		//Э�鳤�Ȳ�����
#define PROTO_LEN_ERR  			 			1005

		//��ֵԽ��
#define VALUE_OUT_OF_RANGE_ERR 	 			1006

		//Ҫ���õ�flag��ԭ��һ�� 
#define FLAY_ALREADY_SET_ERR 	 			1007

		//���ݲ�����
#define VALUE_ISNOT_FULL_ERR 	 			1008

		//ö��Խ��:�������͵��޶���Χ.(��ĳһ�ֶ�ֻ����[0,1],ȴ����2)
#define ENUM_OUT_OF_RANGE_ERR 	 			1009

		//���ر��ĳ���������
#define PROTO_RETURN_LEN_ERR 	 			1010

		//���ݿ�����������
#define DB_DATA_ERR   						1011

#define NET_SEND_ERR   						1012
#define NET_RECV_ERR   						1013
		//KEY �Ѿ�������
#define KEY_EXISTED_ERR 					1014
		//KEY ������
#define KEY_NOFIND_ERR 						1015

		//DB���������
#define DB_CANNOT_UPDATE_ERR   				1016

		//���糬ʱ
#define NET_TIMEOUT_ERR 					1017

		//��Ȼ�����ÿ���ӳ����������
#define CMD_EXEC_MAX_PER_MINUTE_ERR 		1018

#define VALUE_NOENOUGH_E 					1020
#define VALUE_MAX_E 						1021


		//��Ȼ�����md5_tags���ʧ��
#define CMD_MD5_TAG_ERR 					1021

		//����ʱ������û������������
#define CHECK_PASSWD_ERR					1103

		//��insert�����׺��Ѵ���
#define USER_ID_EXISTED_ERR				 	1104
		//��select update, delete ʱ�����׺Ų�����
#define USER_ID_NOFIND_ERR				  	1105

#define LIST_ID_EXISTED					 	1109
		//�б��е�ID  �����б�������
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
		E_LIB_ERR_CODE_DBPROXY_NO_FIND_CMD = 8193,//route.xml����δ����CMD
		E_LIB_ERR_CODE_DBPROXY_TIME_OUT = 8194,//��ʱ
		E_LIB_ERR_CODE_DBPROXY_MAX = 12287,
	};

}