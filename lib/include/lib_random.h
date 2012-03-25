/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	�����
	brief:		OK
*********************************************************************/

#pragma once

namespace ice{

	class lib_random_t
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
		lib_random_t(const lib_random_t& cr);
		lib_random_t& operator=(const lib_random_t& cr);
	};

}//end namespace ice
