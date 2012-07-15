#!/usr/bin/python
# -*- coding:utf-8 -*-

import parse

#写cpp文件
def gen_cpp(xml_data):
	for struct_data in xml_data:
		
		print struct_data.name, struct_data.desc;
		for field_data in struct_data.fields:
			print "\t", field_data.mode, field_data.name, field_data.type, field_data.desc, field_data.size;

def main():
	xml_data = parse.get_xml_data("../xml/protocol_online.xml");
	gen_cpp(xml_data);

if __name__ == "__main__":
	main();
