/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	日志
	brief:		
*********************************************************************/
#pragma once

#include <stdio.h>

//显示在控制台的消息
#define SHOW_LOG printf

//记录在日志中的消息
/*! system is unusable -- 0 */
#define EMERG_LOG printf
/*! action must be taken immediately -- 1 */
#define ALERT_LOG printf
/*! critical conditions -- 2 */
#define CRIT_LOG printf
/*! error conditions -- 3 */
#define ERROR_LOG printf
/*! warning conditions  -- 4 */
#define WARNING_LOG printf
/*! normal but significant condition -- 5 */
#define NOTICE_LOG printf
/*! informational -- 6 */
#define INFO_LOG printf
/*! debug-level messages -- 7 */
#define DEBUG_LOG printf
/*! trace-level messages -- 8 */
#define TRACE_LOG printf