#include "lib_time.h"
#include "lib_lock.h"
#include "lib_log.h"
#include "lib_util.h"
#include "lib_file.h"
#include "lib_timer.h"

namespace{

	const uint32_t MAX_LOG_CNT = 10000000; //日志文件最大数量
	const uint32_t LOG_BUF_SIZE = 8192; //每条日志的最大字节数
	uint16_t LOG_IDX = 0;//日志中的序号

	struct log_info_t {
		ice::lib_log_t::E_LEVEL  level;	  // default log level
		std::string dir_name;//日志目录
		std::string file_pre_name;//文件名前缀
		bool is_multi_thread;//是否多线程
		ice::lib_lock_mutex_t shift_fd_mutex;//互斥锁
		ice::lib_log_t::E_DEST log_dest;//日志输出目的
		uint32_t logtime_interval_sec;// 每个日志文件记录日志的总时间（秒）
		bool has_init;//是否初始化了
		log_info_t(){
			this->init();
		}
		void init(){
			this->level = ice::lib_log_t::e_lvl_debug;
			this->dir_name.clear();
			this->file_pre_name.clear();
			this->is_multi_thread = false;
			this->log_dest = ice::lib_log_t::e_dest_terminal; // write log to terminal by default
			this->logtime_interval_sec = 0;
			this->has_init = false;
		}
		void lock(){
			if (this->is_multi_thread){
				this->shift_fd_mutex.lock();
			}
		}
		void unlock(){
			if (this->is_multi_thread){
				this->shift_fd_mutex.ulock();
			}
		}
	};
	log_info_t s_log_info;

	const char* s_log_color[] = { "\e[1m\e[35m", "\e[1m\e[33m", 
		"\e[1m\e[34m", "\e[1m\e[31m", "\e[1m\e[32m", "\e[1m\e[36m",
		"\e[1m\e[1m", "\e[1m\e[37m", "\e[1m\e[37m"};
	const char* s_color_end = "\e[m";

	struct fds_t {
		int		seq;//顺序号
		int		opfd;//文件FD
		int		day;//一年中的天数
		std::string base_file_name;//基本文件名
		fds_t(){
			this->init();
		}
		void init(){
			this->seq = 0;
			this->opfd = -1;
			this->day = 0;
			this->base_file_name.clear();
		}
	};
	fds_t s_fds_info[ice::lib_log_t::e_lvl_max];

	/**
	 * @brief	生成日志文件路径
	 * @param	int lvl	日志等级
	 * @param	int seq	日志序号
	 * @param	char * file_name	产生的日志文件路径
	 * @param	const struct tm & t_m	时间
	 */
	inline void gen_log_file_path(int lvl, int seq, char* file_name, const struct tm& t_m)
	{
		assert((lvl >= ice::lib_log_t::e_lvl_emerg) && (lvl < ice::lib_log_t::e_lvl_max));

		if (s_log_info.logtime_interval_sec) {
			time_t t = ::time(0) / s_log_info.logtime_interval_sec * s_log_info.logtime_interval_sec;
			struct tm tmp_tm;
			localtime_r(&t, &tmp_tm);

			sprintf(file_name, "%s/%s%04d%02d%02d%02d%02d", 
				s_log_info.dir_name.c_str(), s_fds_info[lvl].base_file_name.c_str(),
				tmp_tm.tm_year + 1900, tmp_tm.tm_mon + 1, 
				tmp_tm.tm_mday, tmp_tm.tm_hour, tmp_tm.tm_min);
		} else {
			sprintf(file_name, "%s/%s%04d%02d%02d%07d", 
				s_log_info.dir_name.c_str(), s_fds_info[lvl].base_file_name.c_str(),
				t_m.tm_year + 1900, t_m.tm_mon + 1, t_m.tm_mday, seq);
		}
	}

	//************************************
	// Brief:     获取日志序号
	// Returns:   int	日志序号
	// Parameter: int lvl	日志等级
	//************************************
	inline int gen_log_seq(int lvl)
	{
		uint32_t seq = 0;
		if (0 != s_log_info.logtime_interval_sec){
			seq = ice::lib_time_t::get_now_second() / s_log_info.logtime_interval_sec;
		}else{
			char file_name[FILENAME_MAX];

			struct tm t_m;
			ice::lib_time_t::get_now_tm(t_m);

			for (; seq != MAX_LOG_CNT; ++seq) {
				gen_log_file_path(lvl, seq, file_name, t_m);
				if (::access(file_name, F_OK) == -1) {
					break;
				}
			}
			seq = (seq ? (seq - 1) : 0);
		}
		return seq;
	}

	int open_fd(int lvl, const struct tm& t_m)
	{
		//O_APPEND 有该选项,write时是线程安全的.请看本页write函数
		int flag = O_WRONLY | O_CREAT | O_APPEND/* | O_LARGEFILE*/;

		char file_name[FILENAME_MAX];
		gen_log_file_path(lvl, s_fds_info[lvl].seq, file_name, t_m);

		s_fds_info[lvl].opfd = ::open(file_name, flag, 0644);
		if (-1 != s_fds_info[lvl].opfd) {
			if (s_fds_info[lvl].day != t_m.tm_yday ) {
				s_fds_info[lvl].day = t_m.tm_yday;
			}

			flag  = ::fcntl(s_fds_info[lvl].opfd, F_GETFD, 0);
			flag |= FD_CLOEXEC;
			::fcntl(s_fds_info[lvl].opfd, F_SETFD, flag);
		}

		return s_fds_info[lvl].opfd;
	}

	//************************************
	// Brief:     切换打开要写入文件的FD
	// Returns:   int -1:ERROR >0:返回打开的文件的FD,
	// Parameter: int lvl
	// Parameter: const struct tm * tm
	//************************************
	int shift_fd(int lvl, const struct tm& t_m)
	{
		s_log_info.lock();

		if ( unlikely((s_fds_info[lvl].opfd < 0) && (open_fd(lvl, t_m) < 0)) ) {
			s_log_info.unlock();
			return -1;
		}

		//off_t length = lseek(fds_info[lvl].opfd, 0, SEEK_END);
		if (s_log_info.logtime_interval_sec) {
			if (likely(s_fds_info[lvl].seq == gen_log_seq(lvl))) {//todo 判断文件大于上限是否重新打开一个文件
				s_log_info.unlock();
				return 0;
			}
		} else {
			if (likely((s_fds_info[lvl].day == t_m.tm_yday))) {//todo 判断文件大于上限是否重新打开一个文件
				s_log_info.unlock();
				return 0;
			}
		}

		ice::lib_file_t::close_fd(s_fds_info[lvl].opfd);

		if (s_log_info.logtime_interval_sec) {
			s_fds_info[lvl].seq = gen_log_seq(lvl);
		} else {
			// if logfile recycle is not used
			if (s_fds_info[lvl].day != t_m.tm_yday) {
				s_fds_info[lvl].seq = 0;
			} else {
				s_fds_info[lvl].seq = (s_fds_info[lvl].seq + 1) % MAX_LOG_CNT;
			}
		}

		int ret = open_fd(lvl, t_m);
		s_log_info.unlock();
		return ret;
	}

}//end of namespace 

int ice::lib_log_t::setup_by_time( const char* dir, E_LEVEL lvl, const char* pre_name, uint32_t logtime )
{
	int ret_code = -1;
	assert(logtime <= 30000000);

	s_log_info.logtime_interval_sec = logtime * 60;

	if (NULL == dir || (0 == ::strlen(dir))) {
		::fprintf(::stderr, "init log dir is NULL!!!\n");
		ret_code = 0;
		goto loop_return;
	}

	if ((lvl < e_lvl_emerg) || (lvl >= e_lvl_max)) {
		::fprintf(::stderr, "init log error, invalid log level=%d\n", lvl);
		goto loop_return;
	}

	//必须可写
	if (0 != ::access(dir, W_OK)) {
		::fprintf(stderr, "access log dir %s error\n", dir);
		goto loop_return;
	}

	//填写  日志等级/目录/文件名前缀
	s_log_info.level = lvl;
	s_log_info.dir_name = dir;
	if (NULL == pre_name){
		s_log_info.file_pre_name.clear();
	}else{
		s_log_info.file_pre_name = pre_name;
	}

	for (int i = e_lvl_emerg; i < e_lvl_max; i++) {
		static const char* log_names[] = { "emerg", "alert", "crit", 
			"error", "warn", "notice", "info", "debug", "trace" };
		s_fds_info[i].init();
		s_fds_info[i].base_file_name = s_log_info.file_pre_name + log_names[i];
		s_fds_info[i].seq  = gen_log_seq(i);
		if (s_fds_info[i].seq < 0) {
			goto loop_return;
		}
	}

	s_log_info.log_dest = e_dest_file;
	s_log_info.has_init = true;
	ret_code    = 0;

loop_return:
	BOOT_LOG(ret_code, "log [dir:%s, per_name:%s, interval_time:%u]", dir, pre_name, logtime);
}

void ice::lib_log_t::destroy()
{
	assert(s_log_info.has_init);

	s_log_info.init();

	for (uint32_t i = 0; i < get_arr_num(s_fds_info); i++){
		ice::lib_file_t::close_fd(s_fds_info[i].opfd);
		s_fds_info[i].init();
	}
}

void ice::lib_log_t::enable_multi_thread()
{
	s_log_info.is_multi_thread = true;
}

void ice::lib_log_t::set_dest( E_DEST dest )
{
	assert(s_log_info.has_init);
	s_log_info.log_dest = dest;
}

void ice::lib_log_t::write( int lvl,uint32_t key, const char* fmt, ... )
{
	if (lvl > s_log_info.level) {
		return;
	}

	va_list ap;

	const struct tm* t_m = lib_timer_t::get_now_tm();
	
	::va_start(ap, fmt);

	if (unlikely(!s_log_info.has_init || (e_dest_terminal & s_log_info.log_dest))) {
		va_list aq;
		::va_copy(aq, ap);
		switch (lvl) {
		case e_lvl_emerg:
		case e_lvl_alert:
		case e_lvl_crit:		
		case e_lvl_error:
			::fprintf(stderr, "%s%02d:%02d:%02d ", s_log_color[lvl],
				t_m->tm_hour, t_m->tm_min, t_m->tm_sec);
			::vfprintf(stderr, fmt, aq);
			::fprintf(stderr, "%s", s_color_end);
			break;
		default:
			::fprintf(stdout, "%s%02d:%02d:%02d ", s_log_color[lvl],
				t_m->tm_hour, t_m->tm_min, t_m->tm_sec);
			::vfprintf(::stdout, fmt, aq);
			::fprintf(::stdout, "%s", s_color_end);
			break;
		}
		::va_end(aq);
	}

	if (unlikely(!(s_log_info.log_dest & e_dest_file) || (shift_fd(lvl, *t_m) < 0))) {
		::va_end(ap);
		return;
	}

	char log_buffer[LOG_BUF_SIZE];
	int pos = ::snprintf(log_buffer, sizeof(log_buffer), "[%02d:%02d:%02d] %u [%05d,%05d]",
		t_m->tm_hour, t_m->tm_min, t_m->tm_sec, key, ::getpid(), LOG_IDX++);
	int end = ::vsnprintf(log_buffer + pos, sizeof(log_buffer) - pos, fmt, ap);
	::va_end(ap);

	//todo 判断返回值(会被信号中断等处理方式).. on linux :  man write 
	//多线程写文件,这里不需要上锁!请 man 2 write 
	// If the file  was open(2)ed  with  O_APPEND,  
	// the file offset is first set to the end of the file before writing.
	// The adjustment of the file offset and the write operation are performed as an atomic step.
	::write(s_fds_info[lvl].opfd, log_buffer, end + pos);
}

void ice::lib_log_t::write_sys( int lvl, const char* fmt, ... )
{
	if (lvl > s_log_info.level) {
		return;
	}

	va_list ap;
	const struct tm* t_m = lib_timer_t::get_now_tm();

	::va_start(ap, fmt);

	if (unlikely(!s_log_info.has_init || e_dest_terminal & s_log_info.log_dest)) {
		switch (lvl) {
		case e_lvl_emerg:
		case e_lvl_alert:
		case e_lvl_crit:		
		case e_lvl_error:
			::fprintf(::stderr, "%s%02d:%02d:%02d ", s_log_color[lvl],
				t_m->tm_hour, t_m->tm_min, t_m->tm_sec);
			::vfprintf(::stderr, fmt, ap);
			::fprintf(::stderr, "%s", s_color_end);
			break;
		default:
			::fprintf(::stdout, "%s%02d:%02d:%02d ", s_log_color[lvl],
				t_m->tm_hour, t_m->tm_min, t_m->tm_sec);
			::vfprintf(::stdout, fmt, ap);
			::fprintf(::stdout, "%s", s_color_end);
			break;
		}
	}

	if (e_dest_file & s_log_info.log_dest) {		
		char log_buffer[LOG_BUF_SIZE];
		int pos = ::snprintf(log_buffer, LOG_BUF_SIZE, "[%02d:%02d:%02d][%05d]",
			t_m->tm_hour, t_m->tm_min, t_m->tm_sec, ::getpid());
		::vsnprintf(log_buffer + pos, LOG_BUF_SIZE - pos, fmt, ap);
		::syslog(lvl, "%s", log_buffer);
	}

	va_end(ap);
}

void ice::lib_log_t::boot( int ok, int space, const char* fmt, ... )
{
	int end, pos;
	va_list ap;

	char log_buffer[LOG_BUF_SIZE];

	::va_start(ap, fmt);
	end = ::vsprintf(log_buffer, fmt, ap);
	::va_end(ap);
	static const int SCREEN_COLS = 80;
	pos = SCREEN_COLS - 10 - (end - space) % SCREEN_COLS;
	int i = 0;
	for (; i < pos; i++){
		log_buffer[end + i] = ' ';
	}
	log_buffer[end + i] = '\0';
	static const char* BOOT_OK = "\e[1m\e[32m[ T O(∩_∩)O ]\e[m";
	static const char* BOOT_FAIL = "\e[1m\e[31m[ F ╮(╯▽╰)╭ ]\e[m";
	::strcat(log_buffer, ok == 0 ? BOOT_OK : BOOT_FAIL);
	::printf("\r%s\n", log_buffer);

	if (0 != ok){
		::exit(ok);
	}
}
