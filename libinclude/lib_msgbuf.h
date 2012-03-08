/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	消息包打包与解析(自动转换字节序、主机-》网络/网络-》主机)
	brief:		
*********************************************************************/

#pragma once

#include <netinet/in.h>

#include <lib_packer.h>
#include <lib_cli_proto.h>
namespace ice{
	//////////////////////////////////////////////////////////////////////////
	//发送数据
	class lib_send_data_t
	{
	public:
		lib_send_data_t(uint16_t cmd,int writepos)
			:cmd_id(cmd),write_pos(writepos){}
		virtual void init_header(uint32_t userid,uint32_t ret = 0) = 0;
		uint8_t* data(){
			return m_storage;
		}
		uint16_t get_cmd(){
			return cmd_id;
		}
		uint32_t len(){
			return write_pos;
		}
		//打包数据
		lib_send_data_t &operator<<(uint32_t value) {
			lib_packer_t::pack(m_storage, value, write_pos);
			return *this;      
		}                      
		lib_send_data_t &operator<<(uint16_t value) {
			lib_packer_t::pack(m_storage, value, write_pos);
			return *this;      
		}                      
		lib_send_data_t &operator<<(uint8_t value) {
			lib_packer_t::pack(m_storage, value, write_pos);
			return *this;      
		}                      
		lib_send_data_t &operator<<(char value) {
			lib_packer_t::pack(m_storage, value, write_pos);
			return *this;      
		}                      
		lib_send_data_t &operator<<(int value) {
			lib_packer_t::pack(m_storage, value, write_pos);
			return *this;      
		}                      
		lib_send_data_t &operator<<(uint64_t value) {
			lib_packer_t::pack(m_storage, value, write_pos);
			return *this;      
		}
		void set_count(uint32_t value, int pos) {
			lib_packer_t::pack(m_storage, value, pos);
		}
		void pack_str(char*mstring, size_t length) {
			lib_packer_t::pack(m_storage, mstring, length, write_pos);
		}
	private:
		static const uint32_t PACK_DEFAULT_SIZE = 8192;//去掉包头,大概相等
	protected:

		uint16_t cmd_id;
		int write_pos;//数据写到的位置
		uint8_t m_storage[PACK_DEFAULT_SIZE];   
	private:

	private:
		lib_send_data_t(const lib_send_data_t &cr); // 拷贝构造函数
		lib_send_data_t & operator=( const lib_send_data_t &cr); // 赋值函数
	};


	//发送给客户端的数据
	class lib_send_data_cli_t : public lib_send_data_t
	{
	public:
		lib_send_data_cli_t(uint32_t cmd, uint32_t seq)
			:lib_send_data_t(cmd, sizeof(cli_proto_head_t)){
				seq_num = seq;
		}
		virtual void init_header(uint32_t userid,uint32_t ret = 0){
			const uint32_t all_len = write_pos;
			write_pos = 0;
			*this<<all_len
				<<cmd_id
				<<userid
				<<seq_num
				<<ret;
			write_pos = all_len;
		}
	protected:
		uint32_t seq_num;
	private:
		lib_send_data_cli_t(const lib_send_data_cli_t &cr); // 拷贝构造函数
		lib_send_data_cli_t & operator=( const lib_send_data_cli_t &cr); // 赋值函数
	};

	//////////////////////////////////////////////////////////////////////////
	//接收数据
	class lib_recv_data_t
	{
	public:
		lib_recv_data_t(const void* recvdata, int readpos)
			:recv_data(recvdata),read_pos(readpos){
		}
		const void* data(){
			return recv_data;
		}
		uint32_t get_read_pos(){
			return read_pos;
		}
	protected:
		const void* recv_data;
		int read_pos;
	private:
		lib_recv_data_t(const lib_recv_data_t &cr);
		lib_recv_data_t & operator=( const lib_recv_data_t &cr);
	};

	//接收客户端的数据
	class lib_recv_data_cli_t : public lib_recv_data_t{
	public:
		lib_recv_data_cli_t(const void* recvdata, int readpos = 0)
			:lib_recv_data_t(recvdata, readpos){
		}
	public:
		//////////////////////////////////////////////////////////////////////////
		//解包数据
		lib_recv_data_cli_t &operator>>(uint32_t &value) {
			lib_packer_t::unpack(recv_data, value, read_pos);
			return *this;      
		}                      
		lib_recv_data_cli_t &operator>>(uint16_t &value) {
			lib_packer_t::unpack(recv_data, value, read_pos);
			return *this;      
		}                      
		lib_recv_data_cli_t &operator>>(uint8_t &value) {
			lib_packer_t::unpack(recv_data, value, read_pos);
			return *this;      
		}                      
		lib_recv_data_cli_t &operator>>(char &value) {
			lib_packer_t::unpack(recv_data, value, read_pos);
			return *this;      
		}                      
		lib_recv_data_cli_t &operator>>(int &value) {
			lib_packer_t::unpack(recv_data, value, read_pos);
			return *this;      
		}                      
		lib_recv_data_cli_t &operator>>(uint64_t &value) {
			lib_packer_t::unpack(recv_data, value, read_pos);
			return *this;      
		}
		void unpack_str(void *mstring, size_t length) {
			lib_packer_t::unpack(recv_data, mstring, length, read_pos);
		}
		void* read_data(){
			return ((char*)recv_data + read_pos);
		}
		//前4个字节是整个包长度
		uint32_t get_len(){
			return (ntohl(*(uint32_t*)recv_data));
		}
		uint32_t remain_len() {
			return get_len() - read_pos;
		}
	protected:
	private:
		lib_recv_data_cli_t(const lib_recv_data_cli_t &cr); // 拷贝构造函数
		lib_recv_data_cli_t & operator=( const lib_recv_data_cli_t &cr); // 赋值函数
	};

}//end namespace ice               
