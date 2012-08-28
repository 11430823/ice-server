#!/usr/bin/python
# -*- coding:utf-8 -*-

import sys
reload(sys)
sys.setdefaultencoding('utf-8')
sys.path.append("./gen_cpp/")

import cpp

def gen_proto(xml_name, gen_cpp_name, gen_cpp_cmd_name):
	cpp._gen_cpp(xml_name, gen_cpp_name, gen_cpp_cmd_name);

def main():
	gen_proto(r"./xml/protocol_online.xml", r"./gen_cpp/cpp_proto/protocol_online.h", r"./gen_cpp/cpp_proto/protocol_online_cmd.h");

if __name__ == "__main__":
	main();
