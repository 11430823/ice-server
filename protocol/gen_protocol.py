#!/usr/bin/python
# -*- coding:utf-8 -*-

CPP_PROTO = r"./gen_proto/cpp_proto/";
CPP_BIN_PATH = r"./gen_proto/";
CPP_BIN = r"cpp.py";

function get_deal_xml{
	echo $1
	xml_name=$1
	#备份原xml文件
	#log_date=$(date +%Y%m%d_%H%M%S)
	#mv $xml_name ./bak/${xml_name}.$log_date
	cd $cpp_bin_path
	./$cpp_bin $xml_name
	cd -
}

#创建备份文件夹
#[  -d  ./bak ] || mkdir ./bak
#[  -d  ./proto ] || mkdir ./proto
#[  -d  ./python ] || mkdir ./python
#[  -d  ./php ] || mkdir ./php
#[  -d  ./xml ] || mkdir ./xml

#-----------------------------------

get_deal_xml ./xml/protocol_online.xml


