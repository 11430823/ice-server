#pragma once

#include <sys/types.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>

#include "iatomic.h"
enum {
	DATA_BLOCK = 0,
	PAD_BLOCK,
	FIN_BLOCK,	// child informs parent to close a connection
	OPEN_BLOCK,
	CLOSE_BLOCK	// parent informs child that a connection had been closed
};
struct shm_head_t {
	volatile int head;
	volatile int tail;
	atomic_t blk_cnt;
} __attribute__ ((packed));

struct shm_queue_t {
	shm_head_t* addr;
	u_int length;
	int pipe_handles[2];
};

struct shm_block_t {
	uint32_t	id;
	uint32_t	fd;
	uint16_t	length;	// length of the whole shmblock, including data
	char		type;
	uint8_t		data[];
} __attribute__ ((packed));


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
	// Parameter: struct bind_config_t * bc:bind config
	// Parameter: int idx:indicates which pipe fd to close for parent process, or upperbound for child process
	// Parameter: int is_child:indicates child or parent, 0 parent, otherwise child
	//************************************
	void close_pipe(struct bind_config_t* bc, int idx, int is_child);
protected:
private:
};

extern shmq_t g_shmq;