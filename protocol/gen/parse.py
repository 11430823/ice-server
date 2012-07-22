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

#存储每次解析出来的结构体数据	
g_structs_data = [];

#解析XML中struct字段
def _prc_xml_struct(structs_nodes):
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
			g_structs_data.append(sd);

#解析XML中struct字段
def _prc_xml_structs(node):
	structs_nodes = get_xmlnode(node, "structs");
	_prc_xml_struct(structs_nodes);


#获取xml中structs的struct数据
def get_xml_data_structs(file_name):
	g_structs_data[:] = [];
	doc = minidom.parse(file_name);
	ice_protocol_node = doc.documentElement;
	_prc_xml_structs(ice_protocol_node);

#获取xml中protocols的struct数据
def get_xml_data_protocols(file_name):
	g_structs_data[:] = [];
	doc = minidom.parse(file_name);
	ice_protocol_node = doc.documentElement;
	protocol_nodes = get_xmlnode(ice_protocol_node, "protocols");

	for protocols_node in protocol_nodes:
		protocol_node = get_xmlnode(protocols_node, "protocol");
		struct_data = _prc_xml_struct(protocol_node);

#获取xml中protocols的cmd数据
def get_xml_data_protocols_cmd(file_name):
	g_structs_data[:] = [];
	doc = minidom.parse(file_name);
	ice_protocol_node = doc.documentElement;
	protocol_nodes = get_xmlnode(ice_protocol_node, "protocols");
	
	for protocols_node in protocol_nodes:
		nodes = get_xmlnode(protocols_node, "protocol");
		for node in nodes:
			sd = xml_struct.cmd_t();
			sd.id = get_attrvalue(node, "cmd");
			print sd.id;
			sd.name = get_attrvalue(node, "name");
			print sd.name;
			g_structs_data.append(sd);
