/*
 * =====================================================================================
 * 
 *	   Filename:  proto.h
 * 
 *	Description:  
 * 
 *		Version:  1.0
 *		Created:  2007年11月01日 14时10分21秒 CST
 *	   Revision:  none
 *	   Compiler:  gcc
 * 
 *		 Author:  xcwen (xcwen), xcwenn@gmail.com
 *		Company:  NULL
 * 
 * =====================================================================================
 */

#pragma once

#include <inttypes.h>
#include <string.h>
#include <stdlib.h>

#include <lib_msgbuf.h>

//define userid type
typedef uint32_t userid_t;

typedef ice::proto_head_t PROTO_HEADER;

//在cmdid 中数据库识别标志
#define RROTO_ROUTE_FIELD               0xFE00 //使用前7个bit

//是否需要保存交易报文
//有涉及到更新数据库数据的协议，都要保存 
#define NEED_UPDATE                         0x0100

#define PROTO_MAX_SIZE  						 	81920
#define PROTO_HEADER_SIZE  							(sizeof (PROTO_HEADER))

#define RECVBUF_HEADER 	  							((PROTO_HEADER*)recvbuf)
#define SNDBUF_PRIVATE_POS  	  					(((char*)*sendbuf)+PROTO_HEADER_SIZE )
#define RCVBUF_PRIVATE_POS  	  					((char*)recvbuf +PROTO_HEADER_SIZE )

#define RECVBUF_USERID 	  							(RECVBUF_HEADER->id)
#define RECVBUF_GROUPID 	  						(RECVBUF_HEADER->id)
#define RECVBUF_CMDID								(RECVBUF_HEADER->cmd)
#define USER_ID_FROM_RECVBUF  						RECVBUF_USERID
#define RECVLEN_FROM_RECVBUF 	  					(RECVBUF_HEADER->len)
#define PROTOID_FROM_RECVBUF 	  					(RECVBUF_HEADER->seq)
#define PRI_IN_LEN									(RECVLEN_FROM_RECVBUF-PROTO_HEADER_SIZE)
#define PRI_IN_POS  								(typeof(p_in))(RCVBUF_PRIVATE_POS) 
#define PRI_SEND_IN_POS  							(typeof(in))(sendbuf+PROTO_HEADER_SIZE ) 
#define SET_STD_ERR_BUF(sendbuf,sndlen,rcvph,ret)  set_std_return(sendbuf,sndlen,rcvph,ret,0)
inline bool set_std_return_p(char *sendbuf, int * sndlen,
        PROTO_HEADER * rcvph,uint32_t protoid ,int ret, uint32_t private_size )
{
    PROTO_HEADER *ph;
    *sndlen=PROTO_HEADER_SIZE+private_size;
    if (*sndlen>81920000){
        return false; 
    }
    ph=(PROTO_HEADER*)sendbuf;
    memcpy (ph,rcvph,PROTO_HEADER_SIZE);
    ph->len=*sndlen;
    ph->seq=protoid;
    ph->ret=ret;
    return true;
}

inline void set_db_proto_buf( char* send_buf,uint32_t cmdid ,uint32_t userid, uint32_t private_size )
{
	PROTO_HEADER *ph=(PROTO_HEADER*)(send_buf);
	memset(ph,0,PROTO_HEADER_SIZE);
	ph->len=PROTO_HEADER_SIZE+private_size ;
	ph->cmd=cmdid ;
	ph->id=userid;
}

inline bool set_std_return(char **sendbuf, int * sndlen,
		PROTO_HEADER * rcvph,int ret, uint32_t private_size ,
		uint32_t proto_header_len =PROTO_HEADER_SIZE  )
{
	PROTO_HEADER *ph;
	*sndlen=proto_header_len +private_size;
	if (*sndlen>81920000){
		return false; 
	}
	 if (!(*sendbuf =  (char*)malloc(*sndlen))) {
		return false;
     }

	ph=(PROTO_HEADER*)*sendbuf;
	memcpy (ph,rcvph,proto_header_len );
	ph->len=*sndlen;
	ph->ret=ret;
	return true;
}
inline int set_proto_buf(char *sendbuf, uint32_t cmd_id, userid_t userid,
	   char * pri_part, uint32_t pri_size )
{	
	PROTO_HEADER *ph=(PROTO_HEADER *)sendbuf ;
	ph->len=PROTO_HEADER_SIZE+pri_size;
	ph->cmd= cmd_id;
	ph->seq=0x12345678;
	ph->id=userid;
	ph->ret=0;
	memcpy((sendbuf+PROTO_HEADER_SIZE), pri_part,pri_size);
	return ph->len;    
}
inline int set_proto_header(char *sendbuf,  uint32_t cmd_id,uint32_t proto_id,  
		userid_t userid, uint32_t pri_size=0 ,int result=0 )
{	
	PROTO_HEADER *ph=(PROTO_HEADER *)sendbuf ;
	ph->len=PROTO_HEADER_SIZE+pri_size;
	ph->cmd= cmd_id;
	ph->seq=proto_id;
	ph->id=userid;
	ph->ret=0;
	return ph->len;    
}
