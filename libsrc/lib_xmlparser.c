#include "lib_xmlparser.h"

namespace ice {
	int lib_xmlparser::open( const char* name )
	{
		if(NULL != this->doc_ptr){
			return -1;
		}

		//this->doc_ptr = xmlReadFile(name,"UTF-8",XML_PARSE_RECOVER); //解析文件
		xmlKeepBlanksDefault(0);
		this->doc_ptr = xmlReadFile(name, 0, XML_PARSE_NOBLANKS);
		
		//m_doc = xmlParseFile(pszFilePath);
		//检查解析文档是否成功，如果不成功，libxml将指一个注册的错误并停止。
		//一个常见错误是不适当的编码。XML标准文档除了用UTF-8或UTF-16外还可用其它编码保存。
		//如果文档是这样，libxml将自动地为你转换到UTF-8。更多关于XML编码信息包含在XML标准中.
		if (NULL == this->doc_ptr){
			//文档打开错误
			return -1;
		}
		this->node_ptr = xmlDocGetRootElement(this->doc_ptr); //确定文档根元素

		if (NULL == this->node_ptr){
			//空得xml文件
			xmlFreeDoc(this->doc_ptr);
			return -1;
		}
		return 0;
	}

	void lib_xmlparser::move2children_node()
	{
		this->node_ptr = this->node_ptr->xmlChildrenNode;
	}

	void lib_xmlparser::move2next_node()
	{
		this->node_ptr = this->node_ptr->next;
	}

	lib_xmlparser::lib_xmlparser()
	{
		this->doc_ptr = NULL;
	}

	lib_xmlparser::~lib_xmlparser()
	{
		if(NULL != this->doc_ptr){
			xmlFreeDoc(this->doc_ptr);
		}
	}

	bool lib_xmlparser::strcmp( const char* name )
	{
		return xmlStrcmp(this->node_ptr->name,(const xmlChar*)name);
	}

}//end namespace ice

