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
#define LOG_FILE_NAME_PRE_SIZE 32//日志文件名前缀

	struct log_info_t {
		ice::E_LOG_LEVEL  level;	  // default log level
		uint32_t each_file_size_max;//每个日志的最大字节数
		char dir_name[NAME_MAX];//日志目录
		std::string file_pre_name;//文件名前缀
		log_info_t(){
			level = ice::log_lvl_debug;
			memset(dir_name, 0, sizeof(dir_name));
		}
	}log_info;

	ice::E_LOG_DEST s_log_dest = ice::log_dest_terminal; // write log to terminal by default
	int s_multi_thread;
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

	struct fds_t {
		int		seq;
		int		opfd;//文件FD
		int		day;//一年中的天数
		char	base_filename[64];//基本文件名
		int		base_filename_len;//基本文件名长度
		int  	cur_day_seq_count;//当天轮转文件的个数
	} fds_info[ice::log_lvl_max];

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
		assert((lvl >= ice::log_lvl_emerg) && (lvl < ice::log_lvl_max));

		if (s_logtime_interval) {
			time_t t = time(0) / s_logtime_interval * s_logtime_interval;
			struct tm tmp_tm;
			localtime_r(&t, &tmp_tm);

			sprintf(file_name, "%s/%s%04d%02d%02d%02d%02d", log_info.dir_name, fds_info[lvl].base_filename,
				tmp_tm.tm_year + 1900, tmp_tm.tm_mon + 1, tmp_tm.tm_mday, tmp_tm.tm_hour, tmp_tm.tm_min);
		} else {
			sprintf(file_name, "%s/%s%04d%02d%02d%07d", log_info.dir_name, fds_info[lvl].base_filename,
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
		assert((lvl >= ice::log_lvl_emerg) && (lvl < ice::log_lvl_max));

		sprintf(file_name, "%s%04d%02d%02d%07d", fds_info[lvl].base_filename,
			tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, seq);
	}

	inline int get_log_time()
	{
		return time(0) / s_logtime_interval;
	}

	inline int request_log_seq(int lvl)
	{
		return  get_log_seq_nonrecycle(lvl);
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
			} else {
				fds_info[lvl].cur_day_seq_count ++ ;
			}

			flag  = fcntl(fds_info[lvl].opfd, F_GETFD, 0);
			flag |= FD_CLOEXEC;
			fcntl(fds_info[lvl].opfd, F_SETFD, flag);
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

		//off_t length = lseek(fds_info[lvl].opfd, 0, SEEK_END);
		if (s_logtime_interval) {
			if (likely(fds_info[lvl].seq == get_log_time())) {//todo 判断文件大于上限是否重新打开一个文件
				SHIFT_FD_UNLOCK();
				return 0;
			}
		} else {
			if (likely((fds_info[lvl].day == tm->tm_yday))) {
				SHIFT_FD_UNLOCK();
				return 0;
			}
		}

		close(fds_info[lvl].opfd);

		if (s_logtime_interval) {
			fds_info[lvl].seq = get_log_time();
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

int ice::setup_log_by_time(const char* dir, E_LOG_LEVEL lvl, const char* pre_name, uint32_t logtime)
{
	assert((logtime > 0) && (logtime <= 30000000));

	s_logtime_interval = logtime * 60;

	int ret_code = -1;

	if (!dir || (strlen(dir) == 0)) {
		goto loop_return;
	}

	if ((lvl < log_lvl_emerg) || (lvl >= log_lvl_max)) {
		fprintf(stderr, "init log error, invalid log level=%d\n", lvl);
		goto loop_return;
	}

	//必须可写
	if (0 != access(dir, W_OK)) {
		fprintf(stderr, "access log dir %s error, %m\n", dir);
		goto loop_return;
	}

	log_info.level = lvl;

	strncpy(log_info.dir_name, dir, sizeof(log_info.dir_name) - 1);
	log_info.file_pre_name = pre_name;

	for (int i = log_lvl_emerg; i < log_lvl_max; i++) {
		fds_info[i].base_filename_len
			= snprintf(fds_info[i].base_filename, 
			sizeof(fds_info[i].base_filename), "%s%s",
			log_info.file_pre_name.c_str(), lognames[i]);
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
	BOOT_LOG(ret_code, "Set log dir %s, per file name %s, time %u", dir, pre_name, logtime);
}

void ice::log_set_dest(E_LOG_DEST dest)
{
	assert(s_has_init);
	s_log_dest = dest;
}

void ice::destroy_log()
{
	assert(s_has_init);

	log_info.level     = log_lvl_debug;
	s_log_dest    = log_dest_terminal;
	s_has_init      = 0;
	s_logtime_interval   = 0;

	log_info.file_pre_name.clear();
	memset(log_info.dir_name, 0, sizeof(log_info.dir_name));

	for (int i = log_lvl_emerg; i < log_lvl_max; i++) {
		if (fds_info[i].opfd != -1) {
			close(fds_info[i].opfd);
		}
	}
	memset(fds_info, 0, sizeof(fds_info));
}

void ice::log_enable_multi_thread()
{
	s_multi_thread = 1;
}

void ice::write_log( int lvl,uint32_t key, const char* fmt, ... )
{
	if (lvl > log_info.level) {
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

void ice::write_syslog( int lvl, const char* fmt, ... )
{
	if (lvl > log_info.level) {
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

void ice::boot_log( int ok, int dummy, const char* fmt, ... )
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

