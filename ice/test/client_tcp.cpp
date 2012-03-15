#include <iostream>
#include <stdint.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

using namespace std;

#pragma pack(1)
	/* SERVER和CLIENT的协议包头格式 */
	struct cli_proto_head_t{
		uint32_t len; /* 协议的长度 */
		uint16_t cmd; /* 协议的命令号 */
		uint32_t id; /* 账号 */
		uint32_t seq_num;/* 序列号 */
		uint32_t ret; /* S->C, 错误码 */
		uint8_t body[]; /* 包体信息 */
	};
#pragma pack()

int main()
{
	int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

	sockaddr_in addr;
	addr.sin_family=AF_INET;
	addr.sin_port=htons(6000); //保证字节顺序
	inet_pton(AF_INET, "192.168.1.31", &addr.sin_addr);

	int nResult=connect(fd,(sockaddr*)&addr,sizeof(sockaddr));
	if(0 != nResult){
		cout<<"connect failed"<<endl;
	}
    /*
	char sz_recv[1000];
	int len = recv(fd, sz_recv, 1000, 0);
	cout<<len<<endl;
	cout<<sz_recv<<endl;
    */

	cli_proto_head_t head;
    uint32_t j = 10000;
	head.cmd = htons(1);
	head.id = htonl(102356);
	head.ret = 0;
	head.seq_num = htonl(j);
	head.len = htonl(sizeof(cli_proto_head_t));
    int i = 0;
	//for (i = 0; i < 1000; i++){
		head.seq_num = htonl(j+i);
		send(fd, (char*)&head, sizeof(cli_proto_head_t), 0);
	//}
	char sz_recv[1000];
	int len = recv(fd, sz_recv, 1000, 0);
	cout<<len<<endl;
	cout<<sz_recv<<endl;
	close(fd);

	return 0;
}
