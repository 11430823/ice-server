/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	�����
	brief:		OK
*********************************************************************/

#pragma once

namespace ice{

	class lib_random
	{
	public:
		/**
		* @brief ����������Χ�ڵ�������������Ҫ��������������ڳ���������ʱ�����һ��srand()��
		* @param min �����������С����
		* @param max ����������������
		* @return min��max֮��������������min��max��//return [min,max]
		*/
		static inline int random(int min, int max);
		/**
		* @brief ����������ַ���������һ��32λ�Ĺ�ϣֵ
		* @param p ��Ҫ���ɹ�ϣֵ���ַ���
		* @return �ַ�����Ӧ�Ĺ�ϣֵ
		*/
		static inline int hash(const char* p);
	protected:
		
	private:
		lib_random(const lib_random& cr);
		lib_random& operator=(const lib_random& cr);
	};

}//end namespace ice
