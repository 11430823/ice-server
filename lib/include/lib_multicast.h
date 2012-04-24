/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		�鲥
*********************************************************************/

#pragma once

namespace ice{
	class lib_multicast_t : public lib_net_t
	{
	public:
		lib_multicast_t();
		virtual ~lib_multicast_t(){}
		virtual int send(const void* buf, int total){}
		virtual int recv(void* buf, int bufsize){}

		int create(const char* ip, uint16_t port);

	protected:

	private:
		lib_multicast_t(const lib_multicast_t& cr);
		lib_multicast_t& operator=(const lib_multicast_t& cr);
	private://TODO
		//����ಥ//TODO δ����
		int join_multicast(int s);
		//�˳��鲥//TODO δ����
		int exit_multicast(int s);
		//�ܾ��鲥//TODO δ����
		int refuse_multicast(int s);
	};
}//end namespace ice