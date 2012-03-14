/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		ok
*********************************************************************/

#pragma once

namespace ice{

	class lib_net_util
	{
	public:
		/**
		* @brief ��getnameinfo��getaddrinfo�Ⱥ������ص�EAI_XXX������ת�������Ƶ�EXXX(errno)�����롣
		* @param eai EAI_XXX������
		* @return �������Ƶ�EXXX(errno)������
		*/
		static int eai_to_errno(int eai);
		//************************************
		// Brief:	  close the given fd(if fd is VALID),and set to be INVALID_SOCKET 
		// Returns:   int (0 on success, -1 on error)
		// Parameter: int & s (fd)
		//************************************
		static inline int close_socket(int& s);
	protected:
		
	private:
		lib_net_util(const lib_net_util& cr);
		lib_net_util& operator=(const lib_net_util& cr);
	};
	

	class lib_net
	{
	public:
		lib_net();
		virtual ~lib_net();
	protected:
		int fd;
	private:
		lib_net(const lib_net& cr);
		lib_net& operator=(const lib_net& cr);
	};

	class lib_net_multicast
	{
	public:
		lib_net_multicast();
		//virtual ~lib_net_mcast(){}
		//����ಥ//TODO δ����
		int join_multicast(int s);
		//�˳��鲥//TODO δ����
		int exit_multicast(int s);
		//�ܾ��鲥//TODO δ����
		int refuse_multicast(int s);
	protected:
		
	private:
		lib_net_multicast(const lib_net_multicast& cr);
		lib_net_multicast& operator=(const lib_net_multicast& cr);
	};

}//end namespace ice
