/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	消息包打包与解析(自动转换字节序,根据lib_packer.h 文件中宏定义)
	brief:		ok
*********************************************************************/

#pragma once

#include "lib_byte_swap.h"
#include "lib_packer.h"
#include "lib_proto.h"
#include "lib_util.h"

namespace ice{
	//////////////////////////////////////////////////////////////////////////
	//发送数据
	template <typename T_HEAD_TYPE>
	class lib_send_data_t
	{
		PROTECTED_R(uint32_t, write_pos);//数据写到的位置
		PRIVATE(void*, send_data);
	public:
		lib_send_data_t(void* senddata){
			this->init();
			this->send_data = senddata;
		}
		inline void init(){
			this->write_pos = sizeof(T_HEAD_TYPE);
		}
		//************************************
		// Brief:     设置数据的头结构
		// Warning:		设置后就不能往数据中打包任何数据,否则包头中的总长度需要重新设定
		//************************************
		virtual void set_head(const T_HEAD_TYPE& rhead) = 0;
		inline void* data(){
			return this->send_data;
		}
		inline uint32_t len(){
			return this->write_pos;
		}
		//打包数据
		inline lib_send_data_t& operator<<(uint32_t value){
			lib_packer_t::pack(this->send_data, value, this->write_pos);
			return *this;
		}                    
		inline lib_send_data_t& operator<<(uint16_t value){
			lib_packer_t::pack(this->send_data, value, this->write_pos);
			return *this;
		}                      
		inline lib_send_data_t& operator<<(uint8_t value){
			lib_packer_t::pack(this->send_data, value, this->write_pos);
			return *this;
		}
                    
		inline lib_send_data_t& operator<<(char value){
			lib_packer_t::pack(this->send_data, value, this->write_pos);
			return *this;
		}                     
		inline lib_send_data_t& operator<<(int value){
			lib_packer_t::pack(this->send_data, value, this->write_pos);
			return *this;
		}                   
		inline lib_send_data_t& operator<<(uint64_t value){
			lib_packer_t::pack(this->send_data, value, this->write_pos);
			return *this;
		}
		// 优化为插入数组时,不需要处理数组长度 [3/10/2012 meng]
		template <typename T_COUNT_TYPE>
		inline void set_count(T_COUNT_TYPE value, int pos) {
			lib_packer_t::pack(this->send_data, value, pos);
		}
		inline void pack_str(const char* data, size_t len){
			lib_packer_t::pack(this->send_data, data, len, this->write_pos);
		}
	private:
		lib_send_data_t(const lib_send_data_t& cr); // 拷贝构造函数
		lib_send_data_t& operator=( const lib_send_data_t& cr); // 赋值函数
	};


	//////////////////////////////////////////////////////////////////////////
	//exmaple:使用发送数据端的例子
	//发送给客户端的数据
	class lib_send_data_cli_t : public lib_send_data_t<proto_head_t>
	{
	public:
		lib_send_data_cli_t()
			:lib_send_data_t<proto_head_t>(send_data){
		}
		virtual void set_head(const proto_head_t& rhead){//uint32_t cmd, uint32_t seq, uint32_t userid, uint32_t ret = 0){
			const uint32_t all_len = this->write_pos;
			this->write_pos = 0;
			*this<<all_len
				<<rhead.cmd
				<<rhead.id
				<<rhead.seq
				<<rhead.ret;
			this->write_pos = all_len;
		}
	protected:
	private:
		static const uint32_t PACK_DEFAULT_SIZE = 81920;//去掉包头,大概相等
		char send_data[PACK_DEFAULT_SIZE];
		lib_send_data_cli_t(const lib_send_data_cli_t& cr); // 拷贝构造函数
		lib_send_data_cli_t& operator=( const lib_send_data_cli_t& cr); // 赋值函数
	};

	//////////////////////////////////////////////////////////////////////////
	//接收数据
	class lib_recv_data_t
	{
		PROTECTED_R(const void*, recv_data);
		PROTECTED_R(uint32_t, read_pos);
	public:
		lib_recv_data_t(const void* recvdata, int readpos){
			this->recv_data = recvdata;
			this->read_pos = readpos;
		}
		inline const void* data(){
			return this->recv_data;
		}
		//////////////////////////////////////////////////////////////////////////
		//解包数据
		inline lib_recv_data_t& operator>>(uint32_t& value){
			lib_packer_t::unpack(this->recv_data, value, this->read_pos);
			return *this;
		}                     
		inline lib_recv_data_t& operator>>(uint16_t& value){
			lib_packer_t::unpack(this->recv_data, value, this->read_pos);
			return *this;
		}                     
		inline lib_recv_data_t& operator>>(uint8_t& value){
			lib_packer_t::unpack(this->recv_data, value, this->read_pos);
			return *this;
		}                   
		inline lib_recv_data_t& operator>>(char& value){
			lib_packer_t::unpack(this->recv_data, value, this->read_pos);
			return *this;
		}                     
		inline lib_recv_data_t& operator>>(int& value){
			lib_packer_t::unpack(this->recv_data, value, this->read_pos);
			return *this;
		}                      
		inline lib_recv_data_t &operator>>(uint64_t& value){
			lib_packer_t::unpack(this->recv_data, value, this->read_pos);
			return *this;
		}
		inline void unpack_str(void* mstring, size_t length){
			lib_packer_t::unpack(this->recv_data, mstring, length, this->read_pos);
		}
		inline void* read_pos_data(){
			return ((char*)this->recv_data + this->read_pos);
		}
	private:
		lib_recv_data_t(const lib_recv_data_t& cr);
		lib_recv_data_t& operator=( const lib_recv_data_t& cr);
	};

	//////////////////////////////////////////////////////////////////////////
	//example:使用接收数据的例子
	//接收客户端的数据
	class lib_recv_data_cli_t : public lib_recv_data_t{
	public:
		lib_recv_data_cli_t(const void* recvdata, int readpos = 0)
			:lib_recv_data_t(recvdata, readpos){
		}
	public:
		//前4个字节是整个包长度
		inline uint32_t get_len(){
			return (lib_byte_swap_t::bswap((uint32_t)(*(uint32_t*)this->recv_data)));
		}
		inline uint32_t remain_len() {
			return get_len() - this->read_pos;
		}
	protected:
	private:
		lib_recv_data_cli_t(const lib_recv_data_cli_t& cr); // 拷贝构造函数
		lib_recv_data_cli_t& operator=( const lib_recv_data_cli_t& cr); // 赋值函数
	};
}//end namespace ice  



