/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	�����
	brief:		
*********************************************************************/

#pragma once

namespace ice{

	/**
	* @brief ����������Χ�ڵ�������������Ҫ��������������ڳ���������ʱ�����һ��srand()��
	* @param min �����������С����
	* @param max ����������������
	* @return min��max֮��������������min��max��//return [min,max]
	*/
	int random(int min, int max);

	/**
	* @brief ����������ַ���������һ��32λ�Ĺ�ϣֵ
	* @param p ��Ҫ���ɹ�ϣֵ���ַ���
	* @return �ַ�����Ӧ�Ĺ�ϣֵ
	*/
	int hash(const char* p);







}//end namespace ice
