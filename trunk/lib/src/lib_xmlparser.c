#include "lib_xmlparser.h"

namespace ice {
	int lib_xmlparser::open( const char* name )
	{
		if(NULL != this->doc_ptr){
			return -1;
		}

		//this->doc_ptr = xmlReadFile(name, "UTF-8", XML_PARSE_RECOVER); //�����ļ�
		xmlKeepBlanksDefault(0);
		this->doc_ptr = xmlReadFile(name, 0, XML_PARSE_NOBLANKS);
		
		//m_doc = xmlParseFile(pszFilePath);
		//�������ĵ��Ƿ�ɹ���������ɹ���libxml��ָһ��ע��Ĵ���ֹͣ��
		//һ�����������ǲ��ʵ��ı��롣XML��׼�ĵ�������UTF-8��UTF-16�⻹�����������뱣�档
		//����ĵ���������libxml���Զ���Ϊ��ת����UTF-8���������XML������Ϣ������XML��׼��.
		if (NULL == this->doc_ptr){
			//�ĵ��򿪴���
			return -1;
		}
		this->node_ptr = xmlDocGetRootElement(this->doc_ptr); //ȷ���ĵ���Ԫ��

		if (NULL == this->node_ptr){
			//�յ�xml�ļ�
			xmlFreeDoc(this->doc_ptr);
			return -1;
		}
		return 0;
	}
}//end namespace ice

