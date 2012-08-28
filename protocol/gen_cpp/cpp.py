#!/usr/bin/python
# -*- coding:utf-8 -*-

import sys
reload(sys)
sys.setdefaultencoding('utf-8')
import parse
import pro_type

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
	write_line(r"#include <lib_include.h>", 0);
	write_line(r"#include <lib_proto/lib_msg.h>", 0, 2);
	write_line(r"#pragma once", 0, 1);
	write_line(r"#pragma pack(1)", 0, 0);

#写cpp文件
def gen_cpp(xml_data, base_class_name):
	for struct_data in xml_data:
		new_line(2);
		write_line(r"//" + struct_data.desc);
		write_line(r"struct " + struct_data.name + base_class_name + r"{");
		
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

		#read();
		write_line(r"virtual bool read(ice::lib_msg_byte_t& msg_byte){", 1);
		for field_data in struct_data.fields:
			if "single" == field_data.mode:
				if pro_type.is_sys_type(field_data.type):
					write_line(r"if(!msg_byte.read_" + field_data.type + "(this->" + field_data.name + ")) return false;", 2);
				else:
					write_line(r"if(!this->" + field_data.name + ".read(msg_byte)) return false;", 2);
			elif "list" == field_data.mode:
				field_data_name_list_cnt = field_data.name + "_list_cnt__";
				field_data_name_list_item = field_data.name + "_list_item__";
				write_line(r"uint32_t " + field_data_name_list_cnt + ";", 2);
				write_line(r"if(!msg_byte.read_uint32(" + field_data_name_list_cnt + ")) return false;", 2);
				if pro_type.is_sys_type(field_data.type):
					write_line(pro_type.get_type(field_data.type) + " " + field_data_name_list_item + ";", 2);
				else:
					write_line(field_data.type + " " + field_data_name_list_item + ";", 2);
				write_line(r"this->" + field_data.name + ".clear();", 2);
				write_line(r"for(uint32_t i = 0; i < " + field_data_name_list_cnt + "; i++){", 2);
				if pro_type.is_sys_type(field_data.type):
					write_line(r"if(!msg_byte.read_" + field_data.type + "(" + field_data_name_list_item + ")) return false;", 3);
				else:
					write_line(r"if(!" + field_data_name_list_item + ".read(msg_byte)) return false;", 3);
				
				write_line(r"this->" + field_data.name + ".push_back(" + field_data_name_list_item + ");", 3);
				write_line(r"}", 2);
			elif "array" == field_data.mode:
				if pro_type.is_sys_type(field_data.type):
					write_line(r"if(!msg_byte.read_buf((char*)this->" + field_data.name + ", sizeof(" + field_data.name + "))) return false;", 2);
				else:
					write_line(r"error!", 2);
				
		write_line(r"return true;", 2);
		write_line(r"}", 1);
		
		#write();
		write_line(r"virtual bool write(ice::lib_msg_byte_t& msg_byte){", 1);
		for field_data in struct_data.fields:
			if "single" == field_data.mode:
				if pro_type.is_sys_type(field_data.type):
					write_line(r"if(!msg_byte.write_" + field_data.type + "(this->" + field_data.name + ")) return false;", 2);
				else:
					write_line(r"if(!this->" + field_data.name + ".write(msg_byte)) return false;", 2);
			elif "list" == field_data.mode:
				write_line(r"if(!msg_byte.write_uint32(this->" + field_data.name + ".size())) return false;", 2);
				write_line(r"for(uint32_t i = 0; i < this->" + field_data.name + ".size(); i++){", 2);
				if pro_type.is_sys_type(field_data.type):
					write_line("if(!msg_byte.write_" + field_data.type + "(this->" + field_data.name + "[i])) return false;", 3);
				else:
					write_line(r"if(!this->" + field_data.name + "[i].write(msg_byte)) return false;", 3);
				write_line(r"}", 2);
			elif "array" == field_data.mode:
				if pro_type.is_sys_type(field_data.type):
					if "char" != field_data.type and "uint8_t" != field_data.type and "int8_t" != field_data.type:
						write_line(r"error! type", 2);
					else:
						write_line(r"if(!msg_byte.write_buf(this->" + field_data.name + ", sizeof(this->" + field_data.name +"))) return false;", 2);
						
			
		write_line(r"return true;", 2);
		write_line(r"}", 1);
		write_line(r"};");


#写cpp_bind文件
def gen_cpp_cmd(xml_data):
	for cmd_data in xml_data:
		write_line(r"BIND_PROTO_CMD(" + cmd_data.id + r", " + cmd_data.name + r", " + cmd_data.struct_in + r");");

def _gen_cpp(xml_name, gen_cpp_name, gen_cpp_cmd_name):
	global fd;
	
	fd = open(gen_cpp_name, "w");
	gen_cpp_header_detailed();
	parse.get_xml_data_structs(xml_name);
	gen_cpp(parse.g_structs_data, " : public ice::lib_msg_t");
	parse.get_xml_data_protocols(xml_name);
	gen_cpp(parse.g_structs_data, " : public ice::lib_msg_t");
	write_line(r"#pragma pack()", 0, 0);
	fd.close();
	
	fd = open(gen_cpp_cmd_name, "w");
	parse.get_xml_data_protocols_cmd(xml_name);
	gen_cpp_cmd(parse.g_structs_data);
	fd.close();

