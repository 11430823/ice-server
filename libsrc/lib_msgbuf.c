#include "lib_msgbuf.h"


ice::lib_send_data_t& ice::lib_send_data_t::operator<<( uint32_t value )
{
	lib_packer_t::pack(this->send_data, value, this->write_pos);
	return *this;
}

ice::lib_send_data_t::lib_send_data_t( int writepos, uint8_t* senddata ) 
{
	this->write_pos = writepos;
	this->send_data = senddata;
}

uint8_t* ice::lib_send_data_t::data()
{
	return this->send_data;
}

uint32_t ice::lib_send_data_t::len()
{
	return this->write_pos;
}

ice::lib_send_data_t& ice::lib_send_data_t::operator<<( uint16_t value )
{
	lib_packer_t::pack(this->send_data, value, this->write_pos);
	return *this;
}

ice::lib_send_data_t& ice::lib_send_data_t::operator<<( uint8_t value )
{
	lib_packer_t::pack(this->send_data, value, this->write_pos);
	return *this;
}

ice::lib_send_data_t& ice::lib_send_data_t::operator<<( char value )
{
	lib_packer_t::pack(this->send_data, value, this->write_pos);
	return *this;
}

ice::lib_send_data_t& ice::lib_send_data_t::operator<<( int value )
{
	lib_packer_t::pack(this->send_data, value, this->write_pos);
	return *this;
}

ice::lib_send_data_t& ice::lib_send_data_t::operator<<( uint64_t value )
{
	lib_packer_t::pack(this->send_data, value, this->write_pos);
	return *this;
}

void ice::lib_send_data_t::pack_str( char* mstring, size_t length )
{
	lib_packer_t::pack(this->send_data, mstring, length, this->write_pos);
}

ice::lib_recv_data_t::lib_recv_data_t( const void* recvdata, int readpos )
{
	this->recv_data = recvdata;
	this->read_pos = readpos;
}

const void* ice::lib_recv_data_t::data()
{
	return this->recv_data;
}

uint32_t ice::lib_recv_data_t::get_read_pos()
{
	return this->read_pos;
}

ice::lib_recv_data_t& ice::lib_recv_data_t::operator>>( uint32_t& value )
{
	lib_packer_t::unpack(this->recv_data, value, this->read_pos);
	return *this;
}

ice::lib_recv_data_t& ice::lib_recv_data_t::operator>>( uint16_t& value )
{
	lib_packer_t::unpack(this->recv_data, value, this->read_pos);
	return *this;
}

ice::lib_recv_data_t& ice::lib_recv_data_t::operator>>( uint8_t& value )
{
	lib_packer_t::unpack(this->recv_data, value, this->read_pos);
	return *this;
}

ice::lib_recv_data_t& ice::lib_recv_data_t::operator>>( char& value )
{
	lib_packer_t::unpack(this->recv_data, value, this->read_pos);
	return *this;
}

ice::lib_recv_data_t& ice::lib_recv_data_t::operator>>( int& value )
{
	lib_packer_t::unpack(this->recv_data, value, this->read_pos);
	return *this;
}

ice::lib_recv_data_t & ice::lib_recv_data_t::operator>>( uint64_t& value )
{
	lib_packer_t::unpack(this->recv_data, value, this->read_pos);
	return *this;
}

void ice::lib_recv_data_t::unpack_str( void* mstring, size_t length )
{
	lib_packer_t::unpack(this->recv_data, mstring, length, this->read_pos);
}

void* ice::lib_recv_data_t::read_pos_data()
{
	return ((char*)this->recv_data + this->read_pos);
}
