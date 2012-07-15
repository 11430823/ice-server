#!/usr/bin/python
# -*- coding:utf-8 -*-

#字段信息
class field_t:
	def __init__(self):
		#字段模式
		self.mode = "";
		#字段名称
		self.name = "";
		#字段类型
		self.type = "";
		#字段说明
		self.desc = "";
		#字段大小
		self.size = "";

#结构体信息
class struct_t:
	def __init__(self):
		#结构体名称
		self.name = "";
		#结构体说明
		self.desc = "";
		#字段列表,存放field_t
		self.fields = [];