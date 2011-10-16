#include <stddef.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>

#include "shmq.h"
#include "bind_conf.h"
#include "util.h"
#include "net.h"
#include "daemon.h"
#include "service.h"
#include "log.h"

shmq_t g_shmq;
inline struct shm_block_t* tail_mb (const struct shm_queue_t *q)
{
	return (shm_block_t *) ((char *) q->addr + q->addr->tail);
}

void restart_child_process(bind_config_elem_t* bc_elem)
{
	if (is_parent && g_daemon.stop && !g_daemon.restart){
		//在关闭服务器时,防止子进程先收到信号退出,父进程再次创建子进程.
		return;
	}
	
	close(bc_elem->recvq.pipe_handles[1]);
	do_del_conn(bc_elem->sendq.pipe_handles[0], 2);
	do_destroy_shmq(bc_elem);

	g_shmq.create(bc_elem);

	int i = g_bind_conf.get_bind_conf_idx(bc_elem);
	pid_t pid;

	if ( (pid = fork ()) < 0 ) {
//		CRIT_LOG("fork failed: %s", strerror(errno));
	} else if (pid > 0) { //parent process
		g_shmq.close_pipe(&g_bind_conf, i, 0);
		do_add_conn(bc_elem->sendq.pipe_handles[0], fd_type_pipe, 0, bc_elem);
		atomic_set(&g_daemon.child_pids[i], pid);
	} else { //child process
		g_service.worker_process(&g_bind_conf, i, g_bind_conf.get_elem_num());
	}
}

static int align_queue_tail (struct shm_queue_t *q)
{
	struct shm_block_t *pad;
	/*
	 *addr->head < addr->tail condition always is ok at here
	 */
	if (likely (q->addr->head >= q->addr->tail))
		return 0;

	pad = tail_mb (q);
	if (q->length - q->addr->tail < sizeof (shm_block_t)
			|| pad->type == PAD_BLOCK)
		q->addr->tail = sizeof (shm_head_t);

	return 0;
}

int shmq_pop(struct shm_queue_t* q, struct shm_block_t** mb)
{
	//queue is empty
	if (q->addr->tail == q->addr->head) {
		return -1;
	}

	align_queue_tail(q);

	//queue is empty
	if (q->addr->tail == q->addr->head) {
		return -1;
	}

	shm_block_t* cur_mb = tail_mb(q);
	int head_pos = q->addr->head;
#ifdef DEBUG
	//tail block overflow
	if (cur_mb->length < sizeof(struct shm_block_t) ||
		(q->addr->tail < head_pos && q->addr->tail + cur_mb->length > head_pos)
		|| (q->addr->tail > head_pos
		&& q->addr->tail + (int)cur_mb->length > (int)q->length))	{
//			ERROR_LOG("shm_queue tail=%d,head=%d,shm_block_t length=%d", q->addr->tail, head_pos, cur_mb->length);
		exit(-1);
	}
#endif
	if (cur_mb->length > PAGE_SIZE){
		ERROR_LOG("large packet, len=%d", cur_mb->length);
		return -1;
	}

	*mb = cur_mb;

	q->addr->tail += cur_mb->length;

 	TRACE_LOG("pop queue: q=%p length=%d tail=%d head=%d id=%u count=%u fd=%d",
 		q, cur_mb->length, q->addr->tail, q->addr->head, (*mb)->id,
 		(atomic_dec(&q->addr->blk_cnt), atomic_read(&q->addr->blk_cnt)), cur_mb->fd);
	return 0;
}
inline struct shm_block_t *
head_mb (const struct shm_queue_t *q)
{
	return (struct shm_block_t *) ((char *) q->addr + q->addr->head);
}
static int
align_queue_head (struct shm_queue_t *q, const struct shm_block_t *mb)
{
	int tail_pos = q->addr->tail;
	int head_pos = q->addr->head;
	struct shm_block_t *pad;

	int surplus = q->length - head_pos;

	if (unlikely (surplus < mb->length))
	{
		//queue is full
		if (unlikely (tail_pos == sizeof (shm_head_t)))
// 			ERROR_RETURN (("shm_queue is full,head=%d,tail=%d,mb_len=%d",
// 			head_pos, tail_pos, mb->length), -1);
 			return -1;
		//bug
		else if (unlikely (q->addr->tail > head_pos))
		{
			// should be impossible
// 			ERROR_LOG("shm_queue bug, head=%d, tail=%d, mb_len=%d, total_len=%u",
// 				head_pos, tail_pos, mb->length, q->length);
			q->addr->tail = sizeof (shm_head_t);
			q->addr->head = sizeof (shm_head_t);
			//no pad mb
		}
		else if (unlikely (surplus < (int)sizeof (shm_block_t)))
		{
			q->addr->head = sizeof (shm_head_t);
			//pad mb 
		}
		else
		{
			pad = head_mb (q);
			pad->type = PAD_BLOCK;
			pad->length = surplus;
			pad->id = 0;
			q->addr->head = sizeof (shm_head_t);
		}
	}

	return 0;
}
int shmq_push(shm_queue_t* q, shm_block_t* mb, const void* data)
{
	char* next_mb;

	assert(mb->length >= sizeof(shm_block_t));

	if (mb->length > PAGE_SIZE) {
		ERROR_LOG("too large packet, len=%d", mb->length);
		return -1;
	}

	if (align_queue_head(q, mb) == -1) {
		return -1;
	}

	int cnt;
	for (cnt = 0; cnt != 10; ++cnt) {
		//queue is full, (page_size): prevent overwriting the buffer which shmq_pop refers to
		if ( unlikely((q->addr->tail > q->addr->head)
			&& (q->addr->tail < q->addr->head + (int)mb->length + (int)PAGE_SIZE)) ) {
				ALERT_LOG("queue [%p] is full, wait 5 microsecs: [cnt=%d]", q, cnt);
				usleep(5);
		} else {
			break;
		}
	}

	if (unlikely(cnt == 10)) {
		ALERT_LOG("queue [%p] is full.", q);
		return -1;
	}

	next_mb = (char*)head_mb(q);

	memcpy(next_mb, mb, sizeof (shm_block_t));
	if (likely(mb->length > sizeof (shm_block_t)))
		memcpy(next_mb + sizeof (shm_block_t), data, mb->length - sizeof (shm_block_t));

	q->addr->head += mb->length;

	write(q->pipe_handles[1], q, 1);
 	TRACE_LOG("push queue: queue=%p,length=%d,tail=%d,head=%d,id=%u,count=%d,fd=%d",
 		q, mb->length, q->addr->tail, q->addr->head, mb->id,
 		(atomic_inc(&q->addr->blk_cnt), atomic_read(&q->addr->blk_cnt)), mb->fd);
	return 0;
}
namespace {
	int pipe_create(int pipe_handles[2])
	{
		if (pipe (pipe_handles) == -1){
			return -1;
		}

		fcntl (pipe_handles[0], F_SETFL, O_NONBLOCK | O_RDONLY);
		fcntl (pipe_handles[1], F_SETFL, O_NONBLOCK | O_WRONLY);

		fcntl (pipe_handles[0], F_SETFD, FD_CLOEXEC);
		fcntl (pipe_handles[1], F_SETFD, FD_CLOEXEC);

		return 0;
	}

	int create_shmq(struct shm_queue_t *q)
	{
		q->addr = (shm_head_t *) mmap (NULL, q->length, PROT_READ | PROT_WRITE,
			MAP_SHARED | MAP_ANON, -1, 0);
		if (q->addr == MAP_FAILED){
			ALERT_LOG("MMAP FAILED [err:%s]", strerror(errno));
			return -1;
		}

		q->addr->head = sizeof (shm_head_t);
		q->addr->tail = sizeof (shm_head_t);
		atomic_set (&(q->addr->blk_cnt), 0);
		pipe_create (q->pipe_handles);
		return 0;
	}
}//end of namespace

void do_destroy_shmq(bind_config_elem_t* bc_elem)
{
	struct shm_queue_t* q = &(bc_elem->sendq);
	// close send queue
	if ( q->addr ) {
		munmap(q->addr, q->length);
		q->addr = NULL;
	}
	// close receive queue
	q = &(bc_elem->recvq);
	if ( q->addr ) {
		munmap(q->addr, q->length);
		q->addr = NULL;
	}
}

void shmq_destroy(const bind_config_elem_t* exclu_bc_elem, int max_shmq_num)
{
	bind_config_t* bc = &g_bind_conf;

	int i = 0;
	for ( ; i != max_shmq_num; ++i ) {
		if (bc->get_elem(i) != exclu_bc_elem) {
			do_destroy_shmq(bc->get_elem(i));
		}
	}
}

void epi2shm( int fd, struct shm_block_t *mb )
{
	mb->id      = epi.fds[fd].id;
	mb->fd      = fd;
	mb->type    = DATA_BLOCK;
	mb->length  = epi.fds[fd].cb.rcvprotlen + sizeof (struct shm_block_t);
}


int shmq_t::create( struct bind_config_elem_t* p )
{
	p->sendq.length = 1 << 26;//64MB
	p->recvq.length = p->sendq.length;

	int err = create_shmq(&(p->sendq)) | create_shmq(&(p->recvq));
	SHOW_LOG ("Create shared memory queue: %dMB, err:%d\r\n", p->recvq.length / 1024 / 512, err);
	return err;
}

void shmq_t::close_pipe( struct bind_config_t* bc, int idx, int is_child )
{
	if (is_child) {
		// close fds inherited from parent process
		for (int i = 0; i != idx; ++i ) {
			close(bc->get_elem(i)->recvq.pipe_handles[1]);
			close(bc->get_elem(i)->sendq.pipe_handles[0]);
		}
	} else {
		close(bc->get_elem(idx)->recvq.pipe_handles[0]);
		close(bc->get_elem(idx)->sendq.pipe_handles[1]);
	}
}

shm_queue_t::shm_queue_t()
{
	addr = NULL;
	length = 0;
	memset(pipe_handles, 0, sizeof(pipe_handles)/sizeof(pipe_handles[0]));
}
