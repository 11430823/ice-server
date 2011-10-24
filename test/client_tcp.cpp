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
	addr.sin_port=htons(9000); //保证字节顺序
	inet_pton(AF_INET, "10.1.1.142", &addr.sin_addr);

	int nResult=connect(fd,(sockaddr*)&addr,sizeof(sockaddr));
	if(0 != nResult){
		cout<<"connect failed"<<endl;
	}

	cli_proto_head_t head;
	head.cmd = 1;
	head.id = 102356;
	head.ret = 0;
	head.seq_num = 1000;
	head.len = sizeof(cli_proto_head_t);
	for (int i = 0; i < 1000; i++){
		head.seq_num++;
		send(fd, (char*)&head, sizeof(cli_proto_head_t), 0);
	}
	close(fd);

	return 0;
}
