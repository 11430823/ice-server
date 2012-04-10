#include <glib.h>
#include <stdint.h>
#include <sstream>

#include <lib_log.h>
#include <lib_util.h>

#include "bench_conf.h"

bench_conf_t g_bench_conf;

namespace {

	const char* s_bench_config_path = "./bench.ini";

	//************************************
	// Brief:     获取指定组中key的值
	// Returns:   int(0:成功.其它:失败)
	// Parameter: std::string & data(传出值)
	//************************************
	template <typename T>
	int get_val(T& data, GKeyFile* keyfile,
		const char* group_name, const char* key_name)
	{
		gchar **a = NULL;
		gsize len = 0;
		if (NULL == (a = g_key_file_get_string_list (keyfile, 
			group_name, key_name, &len, NULL))){
			return -1;  
		}else{
			std::string val;
			for (uint32_t i = 0; i < len; i++){
				val += a[i];
			}
			ice::convert_from_string(data, val);
		}
		g_strfreev(a);
		return 0;
	}

}//end of namespace

int bench_conf_t::load()
{
	int ret = 0;
	GKeyFile *key = NULL;
	
	if (!g_file_test (s_bench_config_path, G_FILE_TEST_EXISTS)){	
		ALERT_LOG("READ BENCH CONFIG FILE EXISTS ERR");
		ret = -1;
		goto ret;
	}

	key = g_key_file_new();
	if (!g_key_file_load_from_file (key, s_bench_config_path, G_KEY_FILE_NONE, NULL)){
		ALERT_LOG("READ BENCH CONFIG FILE ERR");
		ret = -1;
		goto ret;
	}

	if (0 != get_val(this->liblogic_path, key, "plugin", "liblogic")){
		ret = -1;
		goto ret;
	}
	get_val(this->max_fd_num, key, "common", "max_fd_num");
	get_val(this->is_daemon, key, "common", "is_daemon");
	get_val(this->fd_time_out, key, "common", "fd_time_out");
	get_val(this->page_size_max, key, "common", "page_size_max");
	if (0 != get_val(this->log_dir, key, "log", "dir")){
		ret = -1;
		goto ret;
	}
	get_val(this->log_level, key, "log", "level");
	get_val(this->log_save_next_file_interval_min, key, "log", "save_next_file_interval_min");
	get_val(this->core_size, key, "core", "size");
	get_val(this->restart_cnt_max, key, "core", "restart_cnt_max");
	get_val(this->daemon_tcp_ip, key, "net", "daemon_tcp_ip");
	get_val(this->daemon_tcp_port, key, "net", "daemon_tcp_port");
	get_val(this->daemon_tcp_max_fd_num, key, "net", "daemon_tcp_max_fd_num");
ret:
	if (key){
		g_key_file_free(key);
	}
	return ret;
}

bench_conf_t::bench_conf_t()
{
	this->max_fd_num = 20000;
	this->is_daemon = true;
	this->log_level = 8;
	this->log_save_next_file_interval_min = 0;
	this->fd_time_out = 0;
	this->page_size_max = 8192;
	this->core_size = 2147483648;
	this->restart_cnt_max = 20;
	this->daemon_tcp_port = 0;
	this->daemon_tcp_max_fd_num = 20000;
}

std::string bench_conf_t::get_strval(const char* key, const char* name) const
{
	std::string str;
	GKeyFile *file_key = NULL;

	if (!g_file_test (s_bench_config_path, G_FILE_TEST_EXISTS)){	
		ALERT_LOG("READ BENCH CONFIG FILE EXISTS ERR");
		goto ret_return;
	}

	file_key = g_key_file_new();
	if (!g_key_file_load_from_file (file_key, s_bench_config_path, G_KEY_FILE_NONE, NULL)){
		ALERT_LOG("READ BENCH CONFIG FILE ERR");
		goto ret_return;
	}

	if (0 != get_val(str, file_key, key, name)){
		str.clear();
		goto ret_return;
	}

ret_return:
	if (file_key){
		g_key_file_free(file_key);
	}
	return str;
}
