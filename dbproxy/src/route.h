
#include <lib_xmlparser.h>
#include <lib_util.h>

#pragma once

class route_t
{
	PRIVATE(ice::lib_xmlparser, xml);
public:
	route_t(){}
	virtual ~route_t(){}
	int parser();
protected:
	
private:
	route_t(const route_t& cr);
	route_t& operator=(const route_t& cr);
};

extern route_t g_rotue_t;