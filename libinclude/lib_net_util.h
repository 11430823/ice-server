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
 * @brief 把getnameinfo、getaddrinfo等函数返回的EAI_XXX错误码转换成类似的EXXX(errno)错误码。
 * @param eai EAI_XXX错误码
 * @return 返回类似的EXXX(errno)错误码
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
