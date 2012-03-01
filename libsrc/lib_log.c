#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <dirent.h>
#include <fcntl.h>
#include <syslog.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "lib_log.h"
#include "lib_util.h"

namespace{

#define MAX_LOG_CNT 10000000 //日志文件最大数量
#define LOG_BUF_SIZE 8192 //每条日志的字节数
	lib_log::E_LOG_LEVEL  s_log_level;	  // default log level
	uint32_t s_log_size;//每个日志的最大字节数
	int s_log_max_files;//最大文件数量
	int s_max_log_files;//轮转模式中文件数量
	lib_log::E_LOG_DEST s_log_dest; // write log to terminal by default
	int s_multi_thread;
	char s_log_dir[256];//日志目录
	char s_log_pre[32];//文件名前缀
	int  s_logtime_interval; // 每个日志文件记录日志的总时间（秒）
	int s_has_init;//是否初始化
	const char* lognames[] = { "emerg", "alert", "crit", 
		"error", "warn", "notice",
		"info", "debug", "trace" };
	const char* log_color[] = {
		"\e[1m\e[35m", "\e[1m\e[33m", "\e[1m\e[34m", "\e[1m\e[31m",
		"\e[1m\e[32m", "\e[1m\e[36m", "\e[1m\e[1m", "\e[1m\e[37m",	"\e[1m\e[37m"
	};
	const char* color_end = "\e[m";
	pthread_mutex_t g_shift_fd_mutex = PTHREAD_MUTEX_INITIALIZER;
#define SHIFT_FD_LOCK() \
	if (s_multi_thread){\
		pthread_mutex_lock(&g_shift_fd_mutex);\
	}
#define SHIFT_FD_UNLOCK() \
	if (s_multi_thread){\
		pthread_mutex_unlock(&g_shift_fd_mutex);\
	}

static struct fds_t {
	int		seq;
	int		opfd;//文件FD
	int		day;//一年中的天数
	char	base_filename[64];//基本文件名
	int		base_filename_len;//基本文件名长度
	int  	cur_day_seq_count;//当天轮转文件的个数
} fds_info[lib_log::log_lvl_max];

//************************************
// Brief:     生成日志文件路径
// Returns:   void	
// Parameter: int lvl	日志等级
// Parameter: int seq	日志序号
// Parameter: char * file_name	产生的日志文件路径
// Parameter: const struct tm * tm 时间
//************************************
inline void gen_log_file_path(int lvl, int seq, char* file_name, const struct tm* tm)
{
	assert((lvl >= lib_log::log_lvl_emerg) && (lvl < lib_log::log_lvl_max));

	if (s_logtime_interval) {
		time_t t = time(0) / s_logtime_interval * s_logtime_interval;
		struct tm tmp_tm;
		localtime_r(&t, &tmp_tm);

		sprintf(file_name, "%s/%s%04d%02d%02d%02d%02d", s_log_dir, fds_info[lvl].base_filename,
			tmp_tm.tm_year + 1900, tmp_tm.tm_mon + 1, tmp_tm.tm_mday, tmp_tm.tm_hour, tmp_tm.tm_min);
	} else {
		sprintf(file_name, "%s/%s%04d%02d%02d%07d", s_log_dir, fds_info[lvl].base_filename,
			tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, seq);
	}
}

//************************************
// Brief:     非轮转模式获取日志序号
// Returns:   int	日志序号
// Parameter: int lvl	日志等级
//************************************
int get_log_seq_nonrecycle(int lvl)
{
	char file_name[FILENAME_MAX];

	struct tm tm;
	time_t now = time(0);
	localtime_r(&now, &tm);	

	int seq = 0;
	for (; seq != MAX_LOG_CNT; ++seq) {
		gen_log_file_path(lvl, seq, file_name, &tm);
		if (access(file_name, F_OK) == -1) {
			break;
		}
	}

	return (seq ? (seq - 1) : 0);
}

inline int get_logfile_seqno(const char* filename, int loglvl)
{
	return atoi(&filename[fds_info[loglvl].base_filename_len + 8]);
}

inline void log_file_name(int lvl, int seq, char* file_name, const struct tm* tm)
{
	assert((lvl >= lib_log::log_lvl_emerg) && (lvl < lib_log::log_lvl_max));

	sprintf(file_name, "%s%04d%02d%02d%07d", fds_info[lvl].base_filename,
		tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, seq);
}

int get_log_seq_recycle(int lvl)
{
	char file_name[FILENAME_MAX] = { 0 };

	DIR* dir = opendir(s_log_dir);
	if (!dir) {
		return -1;
	}

	struct dirent* dentry;
	while ((dentry = readdir(dir))) {
		if ( (strncmp(dentry->d_name, fds_info[lvl].base_filename,
			fds_info[lvl].base_filename_len) == 0)
			&& (strcmp(dentry->d_name, file_name) > 0) ) {
				snprintf(file_name, sizeof(file_name), "%s", dentry->d_name);
		}
	}

	closedir(dir);

	struct tm tm;
	time_t now = time(0);
	localtime_r(&now, &tm);

	if (file_name[0] == '\0') {
		log_file_name(lvl, 0, file_name, &tm);
	}

	char* date  = &file_name[fds_info[lvl].base_filename_len];
	char  today[9];
	int   seqno = get_logfile_seqno(file_name, lvl);

	snprintf(today, sizeof(today), "%4d%02d%02d",
		tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
	if (strncmp(today, date, 8)) {
		++seqno;
	}

	return seqno % s_max_log_files;
}

inline int get_log_time()
{
	return time(0) / s_logtime_interval;
}

inline int request_log_seq(int lvl)
{
	return (s_max_log_files ? get_log_seq_recycle(lvl) : get_log_seq_nonrecycle(lvl));
}

void rm_files_by_seqno(int loglvl, int seqno, const struct tm* tm)
{
	char filename[128];

	log_file_name(loglvl, seqno, filename, tm);

	DIR* dir = opendir(s_log_dir);
	if (!dir) {
		return;
	}

	struct dirent* dentry;
	while ((dentry = readdir(dir))) {
		if ( (seqno == get_logfile_seqno(dentry->d_name, loglvl))
			&& (strncmp(dentry->d_name, fds_info[loglvl].base_filename, fds_info[loglvl].base_filename_len) == 0)
			&& (strcmp(filename, dentry->d_name) != 0) ) {
				char filepath[FILENAME_MAX];
				snprintf(filepath, sizeof(filepath), "%s/%s", s_log_dir, dentry->d_name);
				remove(filepath);
				// if there are duplicated seqno caused by the '!dir' above and thus leads to some bugs, we should remove the 'break;' below
				break;
		}
	}

	closedir(dir);
}

int open_fd(int lvl, const struct tm* tm)
{
	int flag = O_WRONLY | O_CREAT | O_APPEND/* | O_LARGEFILE*/;

	char file_name[FILENAME_MAX];
	gen_log_file_path(lvl, fds_info[lvl].seq, file_name, tm);

	fds_info[lvl].opfd = open(file_name, flag, 0644);
	if (fds_info[lvl].opfd != -1) {
		if (  fds_info[lvl].day != tm->tm_yday ) {
			fds_info[lvl].cur_day_seq_count = 1;
			fds_info[lvl].day = tm->tm_yday;
			s_max_log_files = s_log_max_files;//隔天了，恢复日志模式
		} else {
			fds_info[lvl].cur_day_seq_count ++ ;
		}
		if  ( s_max_log_files !=0  //轮转模式
			&&  fds_info[lvl].cur_day_seq_count > s_max_log_files/*可用文件写完了*/){
			//直接关闭轮转模式
			fds_info[lvl].seq = s_max_log_files ;
			s_max_log_files = 0;
		}

		flag  = fcntl(fds_info[lvl].opfd, F_GETFD, 0);
		flag |= FD_CLOEXEC;
		fcntl(fds_info[lvl].opfd, F_SETFD, flag);
		// remove files only if logfile recyle is used
		if (s_max_log_files) {
			rm_files_by_seqno(lvl, fds_info[lvl].seq, tm);
		}
	}

	return fds_info[lvl].opfd;
}

int shift_fd(int lvl, const struct tm* tm)
{
	SHIFT_FD_LOCK();

	if ( unlikely((fds_info[lvl].opfd < 0) && (open_fd(lvl, tm) < 0)) ) {
		SHIFT_FD_UNLOCK();
		return -1;
	}

	off_t length = lseek(fds_info[lvl].opfd, 0, SEEK_END);
	if (s_logtime_interval) {
		if (likely(fds_info[lvl].seq == get_log_time())) {
			SHIFT_FD_UNLOCK();
			return 0;
		}
	} else {
		if (likely(((uint32_t)length < s_log_size) && (fds_info[lvl].day == tm->tm_yday))) {
			SHIFT_FD_UNLOCK();
			return 0;
		}
	}

	close(fds_info[lvl].opfd);

	if (s_logtime_interval) {
		fds_info[lvl].seq = get_log_time();
	} else if (s_max_log_files) {
		// if logfile recycle is used
		fds_info[lvl].seq = (fds_info[lvl].seq + 1) % s_max_log_files;
	} else {
		// if logfile recycle is not used
		if (fds_info[lvl].day != tm->tm_yday) {
			fds_info[lvl].seq = 0;
		} else {
			fds_info[lvl].seq = (fds_info[lvl].seq + 1) % MAX_LOG_CNT;
		}
	}

	int ret = open_fd(lvl, tm);
	SHIFT_FD_UNLOCK();
	return ret;
}

}//end of namespace 

int lib_log::log_init_t(const char* dir, E_LOG_LEVEL lvl, const char* pre_name, int logtime)
{
	assert((logtime > 0) && (logtime <= 30000000));

	s_logtime_interval = logtime * 60;

	int ret = log_init(dir, lvl, 0, 0, pre_name);
	BOOT_LOG(ret, "Set log dir %s, per file size by time", dir);
}

int lib_log::log_init(const char* dir, E_LOG_LEVEL lvl, uint32_t size,
	int maxfiles, const char* pre_name)
{
	int ret_code = -1;
	assert((maxfiles >= 0) && (maxfiles <= MAX_LOG_CNT));

	if (!dir || (strlen(dir) == 0)) {
		goto loop_return;
	}

	if ((lvl < log_lvl_emerg) || (lvl >= log_lvl_max)) {
		fprintf(stderr, "init log error, invalid log level=%d\n", lvl);
		goto loop_return;
	}

	//log file is no larger than 2GB
	static const uint32_t max_size = 1 << 31;
	if ( size > max_size ) {
		fprintf(stderr, "init log error, invalid log size=%d\n", size);
		goto loop_return;
	}

	//必须可写
	if (0 != access(dir, W_OK)) {
		fprintf(stderr, "access log dir %s error, %m\n", dir);
		goto loop_return;
	}

	s_log_level = lvl;
	s_log_size      = size;
	s_log_max_files = maxfiles;
	s_max_log_files = maxfiles;

	strncpy(s_log_dir, dir, sizeof(s_log_dir) - 1);
	if (NULL != pre_name) {
		strncpy(s_log_pre, pre_name, sizeof(s_log_pre) - 1);
	}

	for (int i = log_lvl_emerg; i < log_lvl_max; i++) {
		fds_info[i].base_filename_len
			= snprintf(fds_info[i].base_filename, 
			sizeof(fds_info[i].base_filename), "%s%s",
			s_log_pre, lognames[i]);
		fds_info[i].opfd = -1;
		fds_info[i].seq  = s_logtime_interval ? get_log_time() : request_log_seq(i);
		if (fds_info[i].seq < 0) {
			goto loop_return;
		}
	}

	s_log_dest  = log_dest_file;
	s_has_init    = 1;
	ret_code    = 0;

loop_return:
	BOOT_LOG(ret_code, "Set log dir %s, per file size %g MB", dir, size / 1024.0 / 1024.0);
}

void lib_log::set_log_dest(E_LOG_DEST dest)
{
	assert(s_has_init);
	s_log_dest = dest;
}

lib_log::lib_log()
{
	s_log_level	 = log_lvl_debug;
	s_log_dest = log_dest_terminal;
}

void lib_log::log_fini()
{
	assert(s_has_init);

	s_log_level     = log_lvl_debug;
	s_log_size      = 0;
	s_max_log_files = 0;
	s_log_dest    = log_dest_terminal;
	s_has_init      = 0;
	s_log_max_files = 0;
	s_logtime_interval   = 0;

	memset(s_log_dir, 0, sizeof(s_log_dir));
	memset(s_log_pre, 0, sizeof(s_log_pre));

	for (int i = log_lvl_emerg; i < log_lvl_max; i++) {
		if (fds_info[i].opfd != -1) {
			close(fds_info[i].opfd);
		}
	}
	memset(fds_info, 0, sizeof(fds_info));
}

void lib_log::enable_multi_thread()
{
	s_multi_thread = 1;
}

void lib_log::write_log( int lvl,uint32_t key, const char* fmt, ... )
{
	if (lvl > s_log_level) {
		return;
	}

	va_list ap;
	struct tm tm;
	time_t now = time(0);
	localtime_r(&now, &tm);	
	va_start(ap, fmt);

	if (unlikely(!s_has_init || (s_log_dest & log_dest_terminal))) {
		va_list aq;
		va_copy(aq, ap);
		switch (lvl) {
	case log_lvl_emerg:
	case log_lvl_alert:
	case log_lvl_crit:		
	case log_lvl_error:
		fprintf(stderr, "%s%02d:%02d:%02d ", log_color[lvl],
			tm.tm_hour, tm.tm_min, tm.tm_sec);
		vfprintf(stderr, fmt, aq);
		fprintf(stderr, "%s", color_end);
		break;
	default:
		fprintf(stdout, "%s%02d:%02d:%02d ", log_color[lvl],
			tm.tm_hour, tm.tm_min, tm.tm_sec);
		vfprintf(stdout, fmt, aq);
		fprintf(stdout, "%s", color_end);
		break;
		}
		va_end(aq);
	}

	if (unlikely(!(s_log_dest & log_dest_file) || (shift_fd(lvl, &tm) < 0))) {
		va_end(ap);
		return;
	}

	char log_buffer[LOG_BUF_SIZE];
	int pos = snprintf(log_buffer, LOG_BUF_SIZE, "[%02d:%02d:%02d] %u [%05d]",
		tm.tm_hour, tm.tm_min, tm.tm_sec, key, getpid());
	int end = vsnprintf(log_buffer + pos, LOG_BUF_SIZE - pos, fmt, ap);
	va_end(ap);

	write(fds_info[lvl].opfd, log_buffer, end + pos);
}

void lib_log::write_syslog( int lvl, const char* fmt, ... )
{
	if (lvl > s_log_level) {
		return;
	}

	va_list ap;
	struct tm tm;
	time_t now = time(0);
	localtime_r(&now, &tm);
	va_start(ap, fmt);

	if (unlikely(!s_has_init || (s_log_dest & log_dest_terminal))) {
		switch (lvl) {
		case log_lvl_emerg:
		case log_lvl_alert:
		case log_lvl_crit:		
		case log_lvl_error:
			fprintf(stderr, "%s%02d:%02d:%02d ", log_color[lvl],
				tm.tm_hour, tm.tm_min, tm.tm_sec);
			vfprintf(stderr, fmt, ap);
			fprintf(stderr, "%s", color_end);
			break;
		default:
			fprintf(stdout, "%s%02d:%02d:%02d ", log_color[lvl],
				tm.tm_hour, tm.tm_min, tm.tm_sec);
			vfprintf(stdout, fmt, ap);
			fprintf(stdout, "%s", color_end);
			break;
		}
	}

	if (s_log_dest & log_dest_file) {		
		char log_buffer[LOG_BUF_SIZE];
		int pos = snprintf(log_buffer, LOG_BUF_SIZE, "[%02d:%02d:%02d][%05d]",
			tm.tm_hour, tm.tm_min, tm.tm_sec, getpid());
		vsnprintf(log_buffer + pos, LOG_BUF_SIZE - pos, fmt, ap);
		syslog(lvl, "%s", log_buffer);
	}

	va_end(ap);
}

void lib_log::boot_log( int ok, int dummy, const char* fmt, ... )
{
#define SCREEN_COLS	80
#define BOOT_OK		"\e[1m\e[32m[ OK ]\e[m"
#define BOOT_FAIL	"\e[1m\e[31m[ FAILED ]\e[m"
	int end, i, pos;
	va_list ap;

	char log_buffer[LOG_BUF_SIZE];

	va_start(ap, fmt);
	end = vsprintf(log_buffer, fmt, ap);
	va_end(ap);

	pos = SCREEN_COLS - 10 - (end - dummy) % SCREEN_COLS;
	for (i = 0; i < pos; i++){
		log_buffer[end + i] = ' ';
	}
	log_buffer[end + i] = '\0';

	strcat(log_buffer, ok == 0 ? BOOT_OK : BOOT_FAIL);
	printf("\r%s\n", log_buffer);

	if (ok != 0){
		exit(ok);
	}
}

