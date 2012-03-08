/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	消息包
	brief:		
*********************************************************************/

#pragma once

#include <netinet/in.h>

#include <lib_packer.h>

static const uint32_t PACK_DEFAULT_SIZE = 8192;//去掉包头,大概相等

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

//////////////////////////////////////////////////////////////////////////
//发送数据
class SendData
{
public:
	SendData(uint16_t cmd,int writepos)
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
	SendData &operator<<(uint32_t value) {
		ice::pack(m_storage, value, write_pos);
		return *this;      
	}                      
	SendData &operator<<(uint16_t value) {
		ice::pack(m_storage, value, write_pos);
		return *this;      
	}                      
	SendData &operator<<(uint8_t value) {
		ice::pack(m_storage, value, write_pos);
		return *this;      
	}                      
	SendData &operator<<(char value) {
		ice::pack(m_storage, value, write_pos);
		return *this;      
	}                      
	SendData &operator<<(int value) {
		ice::pack(m_storage, value, write_pos);
		return *this;      
	}                      
	SendData &operator<<(uint64_t value) {
		ice::pack(m_storage, value, write_pos);
		return *this;      
	}
	void set_count(uint32_t value, int pos) {
		ice::pack(m_storage, value, pos);
	}
	void pack_str(char*mstring, size_t length) {
		ice::pack(m_storage, mstring, length, write_pos);
	}

protected:
	uint16_t cmd_id;
	int write_pos;//数据写到的位置
	uint8_t m_storage[PACK_DEFAULT_SIZE];   
private:
private:
	SendData(const SendData &cr); // 拷贝构造函数
	SendData & operator=( const SendData &cr); // 赋值函数
};


//发送给客户端的数据
class SendDataClient : public SendData
{
public:
	SendDataClient(uint32_t cmd, uint32_t seq)
		:SendData(cmd, sizeof(cli_proto_head_t)){
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
	SendDataClient(const SendDataClient &cr); // 拷贝构造函数
	SendDataClient & operator=( const SendDataClient &cr); // 赋值函数
};

typedef SendDataClient SendDataCache;

//////////////////////////////////////////////////////////////////////////
//接收数据
class RecvData
{
public:
	RecvData(const void* recvdata, int readpos)
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
	RecvData(const RecvData &cr);
	RecvData & operator=( const RecvData &cr);
};

//接收客户端的数据
class RecvDataClient : public RecvData{
public:
	RecvDataClient(const void* recvdata, int readpos = 0)
		:RecvData(recvdata, readpos){
	}
public:
	//////////////////////////////////////////////////////////////////////////
	//解包数据
	RecvDataClient &operator>>(uint32_t &value) {
		ice::unpack(recv_data, value, read_pos);
		return *this;      
	}                      
	RecvDataClient &operator>>(uint16_t &value) {
		ice::unpack(recv_data, value, read_pos);
		return *this;      
	}                      
	RecvDataClient &operator>>(uint8_t &value) {
		ice::unpack(recv_data, value, read_pos);
		return *this;      
	}                      
	RecvDataClient &operator>>(char &value) {
		ice::unpack(recv_data, value, read_pos);
		return *this;      
	}                      
	RecvDataClient &operator>>(int &value) {
		ice::unpack(recv_data, value, read_pos);
		return *this;      
	}                      
	RecvDataClient &operator>>(uint64_t &value) {
		ice::unpack(recv_data, value, read_pos);
		return *this;      
	}
	void unpack_str(void *mstring, size_t length) {
		ice::unpack(recv_data, mstring, length, read_pos);
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
	RecvDataClient(const RecvDataClient &cr); // 拷贝构造函数
	RecvDataClient & operator=( const RecvDataClient &cr); // 赋值函数
};

typedef RecvDataClient RecvDataCache;                    
