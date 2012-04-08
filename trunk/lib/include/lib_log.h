/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	ok
	brief:		用于记录日志，一共分9种日志等级。
	注意，每条日志不能超过8000字节。
	如果编译程序时定义宏LOG_USE_SYSLOG，则会利用syslog来记录日志，使用的facility是LOG_USER。
*********************************************************************/

#pragma once

#include <stdint.h>

namespace ice{

	class lib_log_t
	{
	public:
		/**
		 * @enum  E_LEVEL
		 * @brief 日志等级
		 */
		enum E_LEVEL {
			/*! system is unusable -- 0 */
			e_lvl_emerg,
			/*! action must be taken immediately -- 1 */
			e_lvl_alert,//黄色
			/*! critical conditions -- 2 */
			e_lvl_crit,//深蓝
			/*! error conditions -- 3 */
			e_lvl_error,
			/*! warning conditions  -- 4 */
			e_lvl_warning,
			/*! normal but significant condition -- 5 */
			e_lvl_notice,
			/*! informational -- 6 */
			e_lvl_info,
			/*! debug-level messages -- 7 */
			e_lvl_debug,
			/*! trace-level messages -- 8。如果定义了宏LOG_USE_SYSLOG，则log_lvl_trace==log_lvl_debug */
		#ifndef LOG_USE_SYSLOG
			e_lvl_trace,
		#else
			e_lvl_trace = e_lvl_debug,
		#endif
			e_lvl_max
		};
		/**
		 * @enum  log_dest
		 * @brief 日志输出方式
		 */
		enum E_DEST {//使用BIT位来写入数据.
			/*! 仅输出到屏幕  */
			e_dest_terminal	= 1,
			/*! 仅输出到文件 */
			e_dest_file		= 2,
			/*! 既输出到屏幕，也输出到文件 */
			e_dest_both		= 3
		};
	public:
		/**
		* @brief 初始化日志记录功能。按时间周期创建新的日志文件。
		* @param dir 日志保存目录。如果填0，则在屏幕中输出日志。
		* @param lvl 日志输出等级。如果设置为e_lvl_notice，则e_lvl_notice以上等级的日志都不输出。
		* @param pre_name 日志文件名前缀。
		* @param logtime 每个日志文件保存logtime分钟的日志，最大不能超过30000000分钟。
		*				假设logtime为15，则每小时产生4个日志文件，每个文件保存15分钟日志。
		* @see set_dest, enable_multi_thread
		*
		* @return 成功返回0，失败返回-1。
		*/
		static int setup_by_time(const char* dir, E_LEVEL lvl, const char* pre_name, uint32_t logtime);
		/**
		* @brief 销毁日志记录功能，所有打开的日志文件fd都会被关闭。不再需要日志功能时，可以调用这个函数。
		*        或者当你想重新设置日志目录、日志等级等参数数，可以先调用该函数，然后调用setup_by_time。
		*/
		static void destroy();
		/**
		* @brief 这个写日志的库默认不支持多线程！多线程程序写日志的话，需要调用先一下这个函数，否则会有问题。
		*
		*/
		static void enable_multi_thread();
		/**
		* @brief 调用log_init初始化日志功能后，可以调用该函数动态调整日志的输出方式。
		*		如果不调用set_dest的话，则输出方式为setup_by_time时确定的方式。
		*		注意：必须在setup_by_time时指定了日志保存目录，才可以调用该函数。
		* @param dest 日志输出方式
		*
		* @see setup_by_time
		*/
		static void set_dest(E_DEST dest);

		#ifdef __GNUC__
			#define LOG_CHECK_FMT(a,b) __attribute__((format(printf, a, b)))
		#else
			#define LOG_CHECK_FMT(a,b)
		#endif

		static void write(int lvl,uint32_t key, const char* fmt, ...) LOG_CHECK_FMT(3, 4);
		static void write_sys(int lvl, const char* fmt, ...) LOG_CHECK_FMT(2, 3);
		static void boot(int ok, int space, const char* fmt, ...) LOG_CHECK_FMT(3, 4);
	protected:
		
	private:
		lib_log_t(const lib_log_t& cr);
		lib_log_t& operator=(const lib_log_t& cr);
	};

}//end namespace ice

#ifndef LOG_USE_SYSLOG
#define DETAIL(level, key, fmt, args...) \
		ice::lib_log_t::write(level, key, "[%s][%d]%s: " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##args)
#else
#define DETAIL(level, key, fmt, args...) \
		ice::lib_log_t::write_sys(level, "[%s][%d]%s: " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##args)
#endif

#ifndef LOG_USE_SYSLOG
#define SIMPLY(level, key, fmt, args...) \
		ice::lib_log_t::write(level, key, fmt "\n", ##args)
#else
#define SIMPLY(level, key, fmt, args...) \
		ice::lib_log_t::write_sys(level, fmt "\n", ##args)
#endif

#ifndef DISABLE_EMERG_LOG
/**
 * @def EMERG_LOG
 * @brief 输出log_lvl_emerg等级日志。如果定义宏DISABLE_EMERG_LOG，则可以在编译期把EMERG_LOG去除。\n
 *        用法示例：EMERG_LOG("dlopen error, %s", error);
 */
#define EMERG_LOG(fmt, args...) \
		DETAIL(ice::lib_log_t::e_lvl_emerg, 0, fmt, ##args)
/**
 * @def KEMERG_LOG
 * @brief 输出log_lvl_emerg等级日志。比EMERG_LOG多一个参数 key,一般写入userid。如果定义宏DISABLE_EMERG_LOG，则可以在编译期把KEMERG_LOG去除。\n
 *        用法示例：KEMERG_LOG(userid, "dlopen error, %s", error);
 */
#define KEMERG_LOG(key, fmt, args...) \
		DETAIL(ice::lib_log_t::e_lvl_emerg, key, fmt, ##args)
#else
#define EMERG_LOG(fmt, args...)
#define KEMERG_LOG(key, fmt, args...) 
#endif

#ifndef DISABLE_ALERT_LOG
/**
 * @def ALERT_LOG
 * @brief 输出log_lvl_alert等级日志。如果定义宏DISABLE_ALERT_LOG，则可以在编译期把ALERT_LOG去除。\n
 *        用法示例：ALERT_LOG("dlopen error, %s", error);
 */
#define ALERT_LOG(fmt, args...) \
		DETAIL(ice::lib_log_t::e_lvl_alert, 0, fmt, ##args)
/**
 * @def KALERT_LOG
 * @brief 输出log_lvl_alert等级日志。比ALERT_LOG多一个参数 key,一般写入userid。如果定义宏DISABLE_ALERT_LOG，则可以在编译期把KALERT_LOG去除。\n
 *        用法示例：KALERT_LOG(userid, "dlopen error, %s", error);
 */
#define KALERT_LOG(key, fmt, args...) \
		DETAIL(ice::lib_log_t::e_lvl_alert, key, fmt, ##args)
#else
#define ALERT_LOG(fmt, args...)
#define KALERT_LOG(key, fmt, args...) 
#endif

#ifndef DISABLE_CRIT_LOG
/**
 * @def CRIT_LOG
 * @brief 输出log_lvl_crit等级日志。如果定义宏DISABLE_CRIT_LOG，则可以在编译期把CRIT_LOG去除。\n
 *        用法示例：CRIT_LOG("dlopen error, %s", error);
 */
#define CRIT_LOG(fmt, args...) \
		DETAIL(ice::lib_log_t::e_lvl_crit, 0, fmt, ##args)
/**
 * @def KCRIT_LOG
 * @brief 输出log_lvl_crit等级日志。比CRIT_LOG多一个参数 key,一般写入userid。如果定义宏DISABLE_CRIT_LOG，则可以在编译期把KCRIT_LOG去除。\n
 *        用法示例：KCRIT_LOG(userid, "dlopen error, %s", error);
 */
#define KCRIT_LOG(key, fmt, args...) \
		DETAIL(ice::lib_log_t::e_lvl_crit, key, fmt, ##args)
#else
#define CRIT_LOG(fmt, args...)
#define KCRIT_LOG(key, fmt, args...) 
#endif

#ifndef DISABLE_ERROR_LOG
/**
 * @def ERROR_LOG
 * @brief 输出log_lvl_error等级日志。如果定义宏DISABLE_ERROR_LOG，则可以在编译期把ERROR_LOG去除。\n
 *        用法示例：ERROR_LOG("dlopen error, %s", error);
 */
#define ERROR_LOG(fmt, args...) \
		DETAIL(ice::lib_log_t::e_lvl_error, 0, fmt, ##args)
/**
 * @def KERROR_LOG
 * @brief 输出log_lvl_error等级日志。比ERROR_LOG多一个参数 key,一般写入userid。 如果定义宏DISABLE_ERROR_LOG，则可以在编译期把ERROR_LOG去除。\n
 *        用法示例：KERROR_LOG(userid, "dlopen error, %s", error);
 */
#define KERROR_LOG(key, fmt, args...) \
		DETAIL(ice::lib_log_t::e_lvl_error, key, fmt, ##args)
#else
#define ERROR_LOG(fmt, args...)
#define KERROR_LOG(key, fmt, args...) 
#endif

#ifndef DISABLE_WARN_LOG
/**
 * @def WARN_LOG
 * @brief 输出log_lvl_warning等级日志。如果定义宏DISABLE_WARN_LOG，则可以在编译期把WARN_LOG去除。\n
 *        用法示例：WARN_LOG("dlopen error, %s", error);
 */
#define WARN_LOG(fmt, args...) \
		SIMPLY(ice::lib_log_t::e_lvl_warning, 0, fmt, ##args)
/**
 * @def KWARN_LOG
 * @brief 输出log_lvl_warning等级日志。比WARN_LOG多一个参数 key,一般写入userid。如果定义宏DISABLE_WARN_LOG，则可以在编译期把KWARN_LOG去除。\n
 *        用法示例：KWARN_LOG(userid, "dlopen error, %s", error);
 */
#define KWARN_LOG(key, fmt, args...) \
		SIMPLY(ice::lib_log_t::e_lvl_warning, key, fmt, ##args)
#else
#define WARN_LOG(fmt, args...)
#define KWARN_LOG(key, fmt, args...) 
#endif

#ifndef DISABLE_NOTI_LOG
/**
 * @def NOTI_LOG
 * @brief 输出log_lvl_notice等级日志。如果定义宏DISABLE_NOTI_LOG，则可以在编译期把NOTI_LOG去除。\n
 *        用法示例：NOTI_LOG("dlopen error, %s", error);
 */
#define NOTI_LOG(fmt, args...) \
		SIMPLY(ice::lib_log_t::e_lvl_notice, 0, fmt, ##args)
/**
 * @def KNOTI_LOG
 * @brief 输出log_lvl_notice等级日志。比NOTI_LOG多一个参数 key,一般写入userid。 如果定义宏DISABLE_NOTI_LOG，则可以在编译期把KNOTI_LOG去除。\n
 *        用法示例：KNOTI_LOG(userid, "dlopen error, %s", error);
 */
#define KNOTI_LOG(key, fmt, args...) \
		SIMPLY(ice::lib_log_t::e_lvl_notice, key, fmt, ##args)
#else
#define NOTI_LOG(fmt, args...)
#define KNOTI_LOG(key, fmt, args...) 
#endif

#ifndef DISABLE_INFO_LOG
/**
 * @def INFO_LOG
 * @brief 输出log_lvl_info等级日志。如果定义宏DISABLE_INFO_LOG，则可以在编译期把INFO_LOG去除。\n
 *        用法示例：INFO_LOG("dlopen error, %s", error);
 */
#define INFO_LOG(fmt, args...) \
		SIMPLY(ice::lib_log_t::e_lvl_info, 0, fmt, ##args)
/**
 * @def KINFO_LOG
 * @brief 输出log_lvl_info等级日志。比ERROR_LOG多一个参数 key,一般写入userid。如果定义宏DISABLE_INFO_LOG，则可以在编译期把KINFO_LOG去除。\n
 *        用法示例：KINFO_LOG(userid, "dlopen error, %s", error);
 */
#define KINFO_LOG(key, fmt, args...) \
		SIMPLY(ice::lib_log_t::e_lvl_info, key, fmt, ##args)
#else
#define INFO_LOG(fmt, args...)
#define KINFO_LOG(key, fmt, args...) 
#endif

#ifndef DISABLE_DEBUG_LOG
/**
 * @def DEBUG_LOG
 * @brief 输出log_lvl_debug等级日志。如果定义宏DISABLE_DEBUG_LOG，则可以在编译期把DEBUG_LOG去除。\n
 *        用法示例：DEBUG_LOG("dlopen error, %s", error);
 */
#define DEBUG_LOG(fmt, args...) \
		SIMPLY(ice::lib_log_t::e_lvl_debug, 0, fmt, ##args)
/**
 * @def KDEBUG_LOG
 * @brief 输出log_lvl_debug等级日志。比DEBUG_LOG 多一个参数 key,一般写入userid。 如果定义宏DISABLE_DEBUG_LOG，则可以在编译期把KDEBUG_LOG去除。\n
 *        用法示例：KDEBUG_LOG(userid, "dlopen error, %s", error);
 */
#define KDEBUG_LOG(key, fmt, args...) \
		SIMPLY(ice::lib_log_t::e_lvl_debug, key, fmt, ##args)
#else
#define DEBUG_LOG(fmt, args...)
#define KDEBUG_LOG(key, fmt, args...) 
#endif

#ifdef ENABLE_TRACE_LOG
/**
 * @def TRACE_LOG
 * @brief 输出log_lvl_trace等级日志。如果不定义宏ENABLE_TRACE_LOG，则可以在编译期把TRACE_LOG去除。\n
 *        如果编译时定义了宏LOG_USE_SYSLOG，则TRACE_LOG日志会写到DEBUG_LOG日志文件里。\n
 *        用法示例：TRACE_LOG("dlopen error, %s", error);
 */
#define TRACE_LOG(fmt, args...) \
		DETAIL(ice::lib_log_t::e_lvl_trace, 0, fmt, ##args)
/**
 * @def KTRACE_LOG
 * @brief 输出log_lvl_trace等级日志。比KTRACE_LOG多一个参数 key,一般写入userid。如果不定义宏ENABLE_TRACE_LOG，则可以在编译期把KTRACE_LOG去除。\n
 *        如果编译时定义了宏LOG_USE_SYSLOG，则KTRACE_LOG日志会写到KDEBUG_LOG日志文件里。\n
 *        用法示例：KTRACE_LOG(userid, "dlopen error, %s", error);
 */
#define KTRACE_LOG(key, fmt, args...) \
		DETAIL(ice::lib_log_t::e_lvl_trace, key, fmt, ##args)
#else
#define TRACE_LOG(fmt, args...)
#define KTRACE_LOG(key, fmt, args...) 
#endif

/**
 * @def BOOT_LOG
 * @brief 输出程序启动日志到屏幕。如果OK非0，则退出程序；如果OK为0，则返回上一级函数。\n
 *        用法示例：BOOT_LOG(-1, "dlopen error, %s", error);
 */
#define BOOT_LOG(OK, fmt, args...) \
		do { \
			ice::lib_log_t::boot(OK, 0, fmt, ##args); \
			return OK; \
		} while (0)

/**
 * @def BOOT_LOG_VOID
 * @brief 输出程序启动日志到屏幕。如果OK非0，则退出程序；如果OK为0，则返回上一级函数。\n
 *        用法示例：BOOT_LOG_VOID(-1, "dlopen error, %s", error);
 */
#define BOOT_LOG_VOID(OK, fmt, args...) \
	do { \
	ice::lib_log_t::boot(OK, 0, fmt, ##args); \
	return; \
	} while (0)

/**
 * @def BOOT_LOG_SPACE
 * @brief 输出程序启动日志到屏幕。如果OK非0，则退出程序；如果OK为0，则返回上一级函数。n是空格填充个数。\n
 *        用法示例：BOOT_LOG_SPACE(0, 8, "dlopen ok");
 */
#define BOOT_LOG_SPACE(OK, n, fmt, args...) \
		do { \
			ice::lib_log_t::boot(OK, n, fmt, ##args); \
			return OK; \
		} while (0)

/**
 * @def ERROR_RETURN
 * @brief 输出log_lvl_error等级的日志，并且返回Y到上一级函数。\n
 *        用法示例：ERROR_RETURN(-1, ("Failed to Create `mcast_fd`: err=%d %s", errno, strerror(errno)));
 */
#define ERROR_RETURN(ret_, msg_) \
		do { \
			ERROR_LOG msg_; \
			return ret_; \
		} while (0)

/**
 * @def ERROR_RETURN_VOID
 * @brief 输出log_lvl_error等级的日志，并且返回上一级函数。\n
 *        用法示例：ERROR_RETURN("Failed to Create `mcast_fd`: err=%d %s", errno, strerror(errno));
 */
#define ERROR_RETURN_VOID(fmt, args...) \
		do { \
			ERROR_LOG(fmt, ##args); \
			return; \
		} while (0)

/**
 * @def WARN_RETURN
 * @brief 输出log_lvl_warning等级的日志，并且返回ret_到上一级函数。\n
 *        用法示例：WARN_RETURN(-1, ("Failed to Create `mcast_fd`: err=%d %s", errno, strerror(errno)));
 */
#define WARN_RETURN(ret_, msg_) \
		do { \
			WARN_LOG msg_; \
			return (ret_); \
		} while (0)

/**
 * @def WARN_RETURN_VOID
 * @brief 输出log_lvl_warning等级的日志，并且返回上一级函数。\n
 *        用法示例：WARN_RETURN_VOID("Failed to Create `mcast_fd`: err=%d %s", errno, strerror(errno));
 */
#define WARN_RETURN_VOID(fmt, args...) \
		do { \
			WARN_LOG(fmt, ##args); \
			return; \
		} while (0)

/**
 * @def DEBUG_RETURN
 * @brief 输出log_lvl_debug等级的日志，并且返回ret_到上一级函数。\n
 *        用法示例：DEBUG_RETURN(-1, ("Failed to Create `mcast_fd`: err=%d %s", errno, strerror(errno)));
 */
#define DEBUG_RETURN(ret_, msg_) \
		do { \
			DEBUG_LOG msg_; \
			return (ret_); \
		} while (0)

/**
 * @def DEBUG_RETURN_VOID
 * @brief 输出log_lvl_debug等级的日志，并且返回上一级函数。\n
 *        用法示例：DEBUG_RETURN_VOID("Failed to Create `mcast_fd`: err=%d %s", errno, strerror(errno));
 */
#define DEBUG_RETURN_VOID(fmt, args...) \
		do { \
			DEBUG_LOG(fmt, ##args); \
			return; \
		} while (0)
