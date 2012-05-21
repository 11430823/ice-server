/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	消息包打包与解析(自动转换字节序、主机-》网络/网络-》主机)
	brief:		ok
*********************************************************************/

#pragma once

#include "lib_byte_swap.h"
#include "lib_packer.h"

namespace ice{
	//////////////////////////////////////////////////////////////////////////
	//发送数据
	template <typename T_HEAD_TYPE>
	class lib_send_data_t
	{
	public:
		lib_send_data_t(char* senddata){
			this->init();
			this->send_data = senddata;
		}
		void init(){
			this->write_pos = sizeof(T_HEAD_TYPE);
		}
		//************************************
		// Brief:     设置数据的头结构
		// Warning:		设置后就不能往数据中打包任何数据,否则包头中的总长度需要重新设定
		//************************************
		virtual void set_head(const T_HEAD_TYPE& rhead) = 0;
		inline char* data(){
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
		inline void pack_str(const char* mstring, size_t length){
			lib_packer_t::pack(this->send_data, mstring, length, this->write_pos);
		}
	protected:
		int write_pos;//数据写到的位置
	private:
		char* send_data;
	private:
		lib_send_data_t(const lib_send_data_t& cr); // 拷贝构造函数
		lib_send_data_t& operator=( const lib_send_data_t& cr); // 赋值函数
	};

#if 0
	//////////////////////////////////////////////////////////////////////////
	//exmaple:使用发送数据端的例子
#pragma pack(1)
	/* SERVER和CLIENT的协议包头格式 */
	struct cli_proto_head_t {
		uint32_t len; /* 协议的长度 */
		uint16_t cmd; /* 协议的命令号 */
		uint32_t id; /* 账号 */
		uint32_t seq_num;/* 序列号 */
		uint32_t ret; /* S->C, 错误码 */
		uint8_t body[]; /* 包体信息 */
	};
#pragma pack()

	//发送给客户端的数据
	class lib_send_data_cli_t : public ice::lib_send_data_t<cli_proto_head_t>
	{
	public:
		lib_send_data_cli_t()
			:ice::lib_send_data_t<cli_proto_head_t>(send_data){
		}
		virtual void set_head(const cli_proto_head_t& rhead){//uint32_t cmd, uint32_t seq, uint32_t userid, uint32_t ret = 0){
			const uint32_t all_len = this->write_pos;
			this->write_pos = 0;
			*this<<all_len
				<<rhead.cmd
				<<rhead.id
				<<rhead.seq_num
				<<rhead.ret;
			this->write_pos = all_len;
		}
	protected:
	private:
		static const uint32_t PACK_DEFAULT_SIZE = 8192;//去掉包头,大概相等
		char send_data[PACK_DEFAULT_SIZE];
		lib_send_data_cli_t(const lib_send_data_cli_t& cr); // 拷贝构造函数
		lib_send_data_cli_t& operator=( const lib_send_data_cli_t& cr); // 赋值函数
	};

#endif


	//////////////////////////////////////////////////////////////////////////
	//接收数据
	class lib_recv_data_t
	{
	public:
		lib_recv_data_t(const void* recvdata, int readpos){
			this->recv_data = recvdata;
			this->read_pos = readpos;
		}
		inline const void* data(){
			return this->recv_data;
		}
		inline uint32_t get_read_pos(){
			return this->read_pos;
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
	protected:
		const void* recv_data;
		int read_pos;
	private:
		lib_recv_data_t(const lib_recv_data_t& cr);
		lib_recv_data_t& operator=( const lib_recv_data_t& cr);
	};
#if 0
	//////////////////////////////////////////////////////////////////////////
	//example:使用接收数据的例子
	//接收客户端的数据
	class lib_recv_data_cli_t : public ice::lib_recv_data_t{
	public:
		lib_recv_data_cli_t(const void* recvdata, int readpos = 0)
			:ice::lib_recv_data_t(recvdata, readpos){
		}
	public:
		//前4个字节是整个包长度
		uint32_t get_len(){
			return (ice::lib_byte_swap_t::bswap((uint32_t)(*(uint32_t*)this->recv_data)));
		}
		uint32_t remain_len() {
			return get_len() - this->read_pos;
		}
	protected:
	private:
		lib_recv_data_cli_t(const lib_recv_data_cli_t& cr); // 拷贝构造函数
		lib_recv_data_cli_t& operator=( const lib_recv_data_cli_t& cr); // 赋值函数
	};

#endif
}//end namespace ice  



