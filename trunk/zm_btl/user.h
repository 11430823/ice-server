/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	用户信息
	brief:		
*********************************************************************/

#pragma  once

#include <stdint.h>
class User
{
public:
	uint32_t id;
public:
	User(void){}
	//virtual ~User(){}
protected:
	
private:
	User(const User &cr);
	User & operator=( const User &cr);
};