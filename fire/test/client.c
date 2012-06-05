#include <iostream>
#include <stdint.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>

#include <lib_proto.h>

using namespace std;

int main()
{
	proto_head_t head;
	head.len = sizeof(proto_head_t);
	head.cmd = 1;
	head.id = 102356;
	head.seq = 1;
	head.ret = 0;
	for(int i = 0; i < sizeof(head);i++){
		char* p = (char*)&head + i;
		char a = *(((char*)&head)+i);
		printf("%02hhX ", a);
	}
	printf("\r\n");
	int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

	sockaddr_in addr;
	addr.sin_family=AF_INET;
	addr.sin_port=htons(9001); //保证字节顺序
	
	addr.sin_addr.s_addr = inet_addr("192.168.0.104"); //保证字节顺序
	int ret=connect(fd,(sockaddr*)&addr,sizeof(sockaddr));
	if(ret==-1){
		cout<<"WSACleanup failed with error:"<< endl;
	}

/*
	for (int i = 0; i < 100000; i++){
		head.seq_num++;
		int slen = 	send(fd, (char*)&head, sizeof(cli_proto_head_t), 0);
		if(slen <= 0){
			std::cout<< slen<< std::endl;
		}
	}
*/
	sleep(6);
	return 0;
}
