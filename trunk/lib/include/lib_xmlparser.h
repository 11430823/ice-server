/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	utilities for parsing xml files\n
				使用方法：请参阅test/xml.cpp和xml.xml
				g++ xml.cpp -I/usr/include/libxml2 -lxml2 -lice
	brief:		ok	
*********************************************************************/

#pragma once

#include <sstream>
#include <assert.h>

#include <libxml/tree.h>

#include "lib_util.h"

namespace ice{

	class lib_xmlparser
	{
		PROPERTY_READONLY_BY_REF_DEFAULT(xmlDocPtr, doc_ptr);//定义解析文档指针
		PROPERTY_READONLY_BY_REF_DEFAULT(xmlNodePtr, node_ptr);//定义结点指针(你需要它为了在各个结点间移动)
	public:
		typedef std::ios_base& (&manip_t)(std::ios_base&);
	public:
		lib_xmlparser();
		virtual ~lib_xmlparser();
		//************************************
		// Brief:     打开文件
		//************************************
		int open(const char* name);
		inline void move2children_node();
		inline void move2next_node();
		inline bool strcmp(const char* name);
		/**
		* @brief 把某个xml属性的值读取出来
		* @param val xml的属性值将被读取到val中
		* @param prop xml属性名
		* @param manip 以什么形式解释xml属性值，默认是dec十进制，还可用oct八进制和hex十六进制
		* @note 注意！不要用这个函数来读取xml属性值到字符数组中，否则有越界的危险！如果需要读取字符串，\n
		*       可以用这个函数把字符串读到string对象里，或者用get_xml_prop_raw_str把字符串读到字符数组里。
		* @see get_xml_prop_raw_str, get_xml_prop_def
		*/
		template <typename T>
		inline void get_xml_prop(T& val, const void* prop, manip_t manip = std::dec){
			xmlChar* str;
			if (!this->node_ptr || !(str = xmlGetProp(this->node_ptr, reinterpret_cast<const xmlChar*>(prop)))) {
				assert(0);
			} else {
				std::istringstream iss(reinterpret_cast<const char*>(str));
				iss >> manip >> val;
				xmlFree(str);
				if (!iss) {
					assert(0);
				}
			}
		}
		/**
		* @brief 同get_xml_prop，但如果prop属性不存在的话，会把默认值def赋给val
		* @see get_xml_prop
		*/
		template <typename T1, typename T2>
		inline void get_xml_prop_def(T1& val, const void* prop, const T2& def, manip_t manip = std::dec){
			xmlChar* str;
			if (!this->node_ptr || !(str = xmlGetProp(this->node_ptr, reinterpret_cast<const xmlChar*>(prop)))) {
				val = def;
			} else {
				std::istringstream iss(reinterpret_cast<const char*>(str));
				iss >> manip >> val;
				xmlFree(str);
				if (!iss) {
					assert(0);
				}
			}
		}
		/**
		* @brief 把某个xml属性的值读取到字符数组中
		* @param val xml的属性值将被读取到val中
		* @param prop xml属性名
		* @throws XmlParseError
		* @see get_xml_prop_raw_str_def
		*/
		template <size_t len>
		inline void get_xml_prop_raw_str(char (&val)[len], const void* prop){
			xmlChar* str;
			if (!this->node_ptr || !(str = xmlGetProp(this->node_ptr, reinterpret_cast<const xmlChar*>(prop)))) {
				assert(0);
			} else {
				strncpy(val, reinterpret_cast<char*>(str), len - 1);
				val[len - 1] = '\0';
				xmlFree(str);
			}
		}
		/**
		* @brief 同get_xml_prop_raw_str，但如果prop属性不存在的话，会把默认值def赋给val
		* @see get_xml_prop_raw_str
		*/
		template <size_t len>
		inline void get_xml_prop_raw_str_def(char (&val)[len], const void* prop, const char* def){
			xmlChar* str;
			if (!this->node_ptr || !(str = xmlGetProp(this->node_ptr, reinterpret_cast<const xmlChar*>(prop)))) {
				strncpy(val, def, len - 1);
				val[len - 1] = '\0';
			} else {
				strncpy(val, reinterpret_cast<char*>(str), len - 1);
				val[len - 1] = '\0';
				xmlFree(str);
			}
		}
		//得到一个节点的内容
		template <typename T>
		inline void get_xml_content(T& val){
			xmlChar* str;
			if (!this->node_ptr || !(str = xmlNodeGetContent(this->node_ptr))) {
				assert(0);
			} else {
				std::istringstream iss(reinterpret_cast<const char*>(str));
				iss >> val;
				xmlFree(str);
			}
		}
		/**
		* @brief 把某个xml属性的值读取到数组中
		* @param arr xml的属性值将被读取到arr中
		* @param prop xml属性名
		* @param manip 以什么形式解释xml属性值，默认是dec十进制，还可用oct八进制和hex十六进制
		* @throws XmlParseError
		* @return 读取到数组arr中的值的个数
		* @see get_xml_prop_arr_def
		*/
		template <typename T1, size_t len>
		inline size_t get_xml_prop_arr(T1 (&arr)[len], const void* prop, manip_t manip = std::dec){
			xmlChar* str;
			if (!this->node_ptr || !(str = xmlGetProp(this->node_ptr, reinterpret_cast<const xmlChar*>(prop)))) {
				assert(0);
			} else {
				size_t i = 0;
				std::istringstream iss(reinterpret_cast<const char*>(str));

				while ((i != len) && (iss >> manip >> arr[i])) {
					++i;
				}

				xmlFree(str);
				return i;
			}
		}
		/**
		* @brief 同get_xml_prop_arr，但如果prop属性不存在的话，会把默认值def赋给arr
		* @return 读取到数组arr中的值的个数，如果prop不存在，则返回0
		* @see get_xml_prop_arr
		*/
		template <typename T1, typename T2, size_t len>
		inline size_t get_xml_prop_arr_def(T1 (&arr)[len], const void* prop, const T2& def, manip_t manip = std::dec){
			xmlChar* str;
			if (!this->node_ptr || !(str = xmlGetProp(this->node_ptr, reinterpret_cast<const xmlChar*>(prop)))) {
				for (size_t i = 0; i != len; ++i) {
					arr[i] = def;
				}
				return 0;
			} else {
				size_t i = 0;
				std::istringstream iss(reinterpret_cast<const char*>(str));

				while ((i != len) && (iss >> manip >> arr[i])) {
					++i;
				}

				xmlFree(str);
				return i;
			}
		}
	protected:
	private:
		lib_xmlparser(const lib_xmlparser& cr);
		lib_xmlparser& operator=(const lib_xmlparser& cr);
	};
//////////////////////////////////////////////////////////////////////////
//example
#if 0
int ex_xml( const char* xml_name )
{
	/*例子
	<?xml version="1.0" encoding="UTF-8"?>
	<!-- edited with XMLSpy v2011 (http://www.altova.com) by kevin (home) -->
	<mini name="mini_name">
		<so name="so_name" version ="1">
			<net>net.so</net>
		</so>
	</mini>
	*/
	ice::lib_xmlparser xml;
	int ret = xml.open(xml_name);
	if(0 != ret){
		return ret;
	}
	xml.move2children_node();

	while(xml.get_node_ptr() != NULL){
		//取出节点中的内容
		if (!xmlStrcmp(xml.get_node_ptr()->name, (const xmlChar *)"so")){
			int i;
			xml.get_xml_prop(i,"version");
			//std::cout << i << std::endl; //得到so的version属性值并输出

			xml.move2children_node();
			while(xml.get_node_ptr()){
				if(!xmlStrcmp(xml.get_node_ptr()->name,(const xmlChar*)"net")){
					std::string str;
					xml.get_xml_content(str);
					//std::cout << str << std::endl; //得到so的值并输出				
				}
				xml.move2next_node();
			}
		}
		xml.move2next_node();
	}
	return 0;
}

#endif
}//end namespace ice



