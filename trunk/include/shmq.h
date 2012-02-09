#pragma once

#include <sys/types.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>

#include <ice_lib/iatomic.h>

enum {
	DATA_BLOCK = 0,
	PAD_BLOCK,
	FIN_BLOCK,	// child informs parent to close a connection
	OPEN_BLOCK,
	CLOSE_BLOCK	// parent informs child that a connection had been closed
};

enum {
	E_PIPE_INDEX_RDONLY = 0,
	E_PIPE_INDEX_WRONLY = 1,
	E_PIPE_INDEX_MAX,
};

#pragma pack(1)
struct shm_head_t {
	volatile int head;//初始化时结构体大小sizeof (shm_head_t)
	volatile int tail;//初始化时结构体大小sizeof (shm_head_t)
	atomic_t blk_cnt;//初始化时 0 
};

struct shm_queue_t {
	shm_head_t* addr;
	u_int length;
	int pipe_handles[E_PIPE_INDEX_MAX];
	shm_queue_t();
};

struct shm_block_t {
	uint32_t	id;
	uint32_t	fd;
	uint16_t	length;	// length of the whole shmblock, including data
	char		type;
	uint8_t		data[];
};
#pragma pack()

void shmq_destroy(const struct bind_config_elem_t* exclu_bc_elem, int max_shmq_num);
int shmq_push(shm_queue_t* q, shm_block_t* mb, const void* data);

void epi2shm(int fd, struct shm_block_t *mb); 
int shmq_pop(shm_queue_t* q, shm_block_t** mb);
void restart_child_process(bind_config_elem_t* bc_elem);
void do_destroy_shmq(bind_config_elem_t* bc_elem);

class shmq_t
{
public:
	int create(struct bind_config_elem_t* p);
	//************************************
	// Brief:     close needless pipe fd
	// Returns:   void
	// Parameter: int idx:indicates which pipe fd to close for parent process, or upperbound for child process
	//************************************
	void close_pipe(int idx, bool is_child);
protected:
private:
	static const u_int m_shmq_max_len = 1 << 26;//64MB//todo 加载到配置项.
};

extern shmq_t g_shmq;