#!/usr/bin/python
# -*- coding:utf-8 -*-

import sys
reload(sys)
sys.setdefaultencoding('utf-8')

import parse

FILE_NAME = r"../xml/protocol_online.xml";
CPP_NAME = r"../cpp/protocol_online.h";

fd = open(CPP_NAME, "w");

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

#写cpp文件
def gen_cpp(xml_data):
	
	write_line(r"#include <lib_include.h>", 0, 2);
	write_line(r"#pragma once", 0, 0);
	for struct_data in xml_data:
		new_line(2);
		write_line(r"//" + struct_data.desc);
		write_line(r"struct " + struct_data.name + r"{");
		
		init_string = [];
		for field_data in struct_data.fields:
			write_line(r"//" + field_data.desc, 1);
			if "single" == field_data.mode:
				write_line(field_data.type + r" " + field_data.name + r";", 1);
				init_string.append(r"this->" + field_data.name + r" = 0;");
			elif "list" == field_data.mode:
				write_line(r"std::vector<" + field_data.type + r"> " + field_data.name + r";", 1);
			elif "array" == field_data.mode:
				write_line(field_data.type + r" " + field_data.name + r"[" + field_data.size + r"];", 1);
		new_line();
		#函数init()
		write_line(r"virtual void init(){", 1);
		for i in init_string:
			write_line(i, 1);
		write_line(r"}", 1);
		#函数xxx
		
		write_line(r"};");

def main():
	xml_data = parse.get_xml_data(FILE_NAME);
	gen_cpp(xml_data);

if __name__ == "__main__":
	main();
