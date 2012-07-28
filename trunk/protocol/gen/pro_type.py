#!/usr/bin/python
# -*- coding:utf-8 -*-

#系统已知类型定义
g_type_conf = {
	"uint32":"uint32_t",
	"uint16":"uint16_t",
	"uint8":"uint8_t",
	"int8":"int8_t",
	"uint64":"uint64_t",
	"int64":"int64_t",
	"int32":"int32_t",
	"int16":"int16_t",
	"char":"char",
};

def is_sys_type(type):
	return g_type_conf.has_key(type);

def get_type(typestr):
	if is_sys_type(typestr):
		return g_type_conf[typestr];
	else:
		return typestr;
		
