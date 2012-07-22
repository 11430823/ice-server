#!/usr/bin/python
# -*- coding:utf-8 -*-

import sys
reload(sys)
sys.setdefaultencoding('utf-8')

import parse

import pro_type

FILE_NAME = r"../xml/protocol_online.xml";
CPP_NAME = r"../cpp/protocol_online.h";
CPP_CMD_NAME = r"../cpp/protocol_online_cmd.h";
#文件FD
fd = "";

def new_line(cnt = 1):
	for i in range(0, cnt):
		fd.write("\r");
		
def new_table(cnt = 1):
	for i in range(0, cnt):
		fd.write("\t");

def write_line(data, tab = 0, line = 1):
	new_table(tab);
	fd.write(data);
	new_line(line);

def gen_cpp_header_detailed():
	write_line(r"#include <lib_include.h>", 0, 2);
	write_line(r"#pragma once", 0, 0);

#写cpp文件
def gen_cpp(xml_data):
	for struct_data in xml_data:
		new_line(2);
		write_line(r"//" + struct_data.desc);
		write_line(r"struct " + struct_data.name + r"{");
		
		init_string = [];
		for field_data in struct_data.fields:
			write_line(r"//" + field_data.desc, 1);
			if "single" == field_data.mode:
				write_line(pro_type.get_type(field_data.type) + r" " + field_data.name + r";", 1);
				if pro_type.is_sys_type(field_data.type):
					init_string.append(r"this->" + field_data.name + r" = 0;");
			elif "list" == field_data.mode:
				write_line(r"std::vector<" + pro_type.get_type(field_data.type) + r"> " + field_data.name + r";", 1);
				init_string.append(r"this->" + field_data.name + r".clear();");
			elif "array" == field_data.mode:
				write_line(pro_type.get_type(field_data.type) + r" " + field_data.name + r"[" + field_data.size + r"];", 1);
				if pro_type.is_sys_type(field_data.type):
					init_string.append(r"::memset(this->" + field_data.name + r", 0, sizeof(this->" + field_data.name + r"));");
		new_line();
		#初始化函数
		write_line(struct_data.name + r"(){this->init();}", 1);
		
		#函数init()
		write_line(r"virtual void init(){", 1);
		for i in init_string:
			write_line(i, 2);
		write_line(r"}", 1);
		
		write_line(r"};");


#写cpp_bind文件
def gen_cpp_cmd(xml_data):
	for cmd_data in xml_data:
		write_line(r"BIND_PROTO_CMD(" + cmd_data.id + r", " + cmd_data.name + r");");

def main():
	global fd;
	
	fd = open(CPP_NAME, "w");
	gen_cpp_header_detailed();
	parse.get_xml_data_structs(FILE_NAME);
	gen_cpp(parse.g_structs_data);
	parse.get_xml_data_protocols(FILE_NAME);
	gen_cpp(parse.g_structs_data);
	fd.close();

	fd = open(CPP_CMD_NAME, "w");
	parse.get_xml_data_protocols_cmd(FILE_NAME);
	gen_cpp_cmd(parse.g_structs_data);
	fd.close();

if __name__ == "__main__":
	main();
