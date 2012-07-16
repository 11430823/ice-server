#!/usr/bin/python
# -*- coding:utf-8 -*-

from xml.dom import minidom

import xml_struct

def get_attrvalue(node, attrname):
	return node.getAttribute(attrname) if node else '';

def get_nodevalue(node, index = 0):
	return node.childNodes[index].nodeValue if node else '';

def get_xmlnode(node, name):
	return node.getElementsByTagName(name) if node else [];
	


def get_xml_data(file_name):
	doc = minidom.parse(file_name);
	ice_protocol_node = doc.documentElement;
	structs_nodes = get_xmlnode(ice_protocol_node, "structs");

	structs_data = [];
	
	for structs_node in structs_nodes:
		struct_node = get_xmlnode(structs_node, "struct");
		for node in struct_node:
			sd = xml_struct.struct_t();
			sd.name = get_attrvalue(node, "name");
			sd.desc = get_attrvalue(node, "desc");

			field_nodes = get_xmlnode(node, "field");
			for field_node in field_nodes:
				f = xml_struct.field_t();
				f.mode = get_attrvalue(field_node, "mode");
				f.name = get_attrvalue(field_node, "name");
				f.type = get_attrvalue(field_node, "type");
				f.desc = get_attrvalue(field_node, "desc");
				f.size = get_attrvalue(field_node, "size");
				
				sd.fields.append(f);

			
			structs_data.append(sd);
	return structs_data;

