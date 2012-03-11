/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	
	brief:		ok
*********************************************************************/

#pragma once

namespace ice{
/**
 * @brief ��getnameinfo��getaddrinfo�Ⱥ������ص�EAI_XXX������ת�������Ƶ�EXXX(errno)�����롣
 * @param eai EAI_XXX������
 * @return �������Ƶ�EXXX(errno)������
 */
	int eai_to_errno(int eai){
		switch (eai) {
		case EAI_ADDRFAMILY:
			return EAFNOSUPPORT;
		case EAI_AGAIN:
			return EAGAIN;
		case EAI_MEMORY:
			return ENOMEM;
		case EAI_SERVICE:
		case EAI_SOCKTYPE:
			return ESOCKTNOSUPPORT;
		case EAI_SYSTEM:
			return errno;
		}
		return EADDRNOTAVAIL;
	}

}//end namespace ice
