#include <glib.h>
#include <stdint.h>
#include <sstream>

#include <ice_lib/log.h>

#include "bench_conf.h"

bench_conf_t g_bench_conf;

namespace {

	const char* bench_config_path = "./bench.ini";

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
			ALERT_LOG("READ BENCH CONFIG FILE KEY ERR [GROUP_NAME:%s, KEY_NAME:%s]",
				group_name, key_name);
			return -1;  
		}else{
			std::string val;
			for (uint32_t i = 0; i < len; i++){
				val += a[i];
			}
			std::stringstream ss(val);
			ss >> data;
		}
		g_strfreev(a);
		return 0;
	}

}//end of namespace

int bench_conf_t::load()
{
	int ret = 0;
	GKeyFile *key = NULL;
	
	if (!g_file_test (bench_config_path, G_FILE_TEST_EXISTS)){	
		ALERT_LOG("READ BENCH CONFIG FILE EXISTS ERR");
		ret = -1;
		goto ret;
	}

	key = g_key_file_new();
	if (!g_key_file_load_from_file (key, bench_config_path, G_KEY_FILE_NONE, NULL)){
		ALERT_LOG("READ BENCH CONFIG FILE ERR");
		ret = -1;
		goto ret;
	}

	if (0 != get_val(m_liblogic_path, key, "plugin", "liblogic")){
		ret = -1;
		goto ret;
	}

	if (0 != get_val(m_max_fd_num, key, "common", "max_fd_num")){
		ret = -1;
		goto ret;
	}
	if (0 != get_val(m_daemon, key, "common", "is_daemon")){
		ret = -1;
		goto ret;
	}

	if (0 != get_val(m_log_dir, key, "log", "dir")){
		ret = -1;
		goto ret;
	}
	if (0 != get_val(m_log_level, key, "log", "level")){
		ret = -1;
		goto ret;
	}
	if (0 != get_val(m_log_max_byte, key, "log", "size")){
		ret = -1;
		goto ret;
	}
	if (0 != get_val(m_log_max_files, key, "log", "max_files")){
		ret = -1;
		goto ret;
	}
ret:
	if (key){
		g_key_file_free(key);
	}
	return ret;
}

std::string bench_conf_t::get_liblogic_path() const
{
	return m_liblogic_path;
}

uint32_t bench_conf_t::get_max_fd_num() const
{
	return m_max_fd_num;
}

bench_conf_t::bench_conf_t()
{
	m_max_fd_num = 0;
	m_daemon = false;
	m_log_level = 0;
	m_log_max_byte = 0;
	m_log_max_files = 0;
	m_log_save_next_file_interval_min = 0;
}

bool bench_conf_t::is_daemon()const
{
	return m_daemon;
}

std::string bench_conf_t::get_strval(std::string& key, std::string& name)
{
	GKeyFile *file_key = NULL;

	std::string str;
	if (!g_file_test (bench_config_path, G_FILE_TEST_EXISTS)){	
		ALERT_LOG("READ BENCH CONFIG FILE EXISTS ERR");
		goto ret_return;
	}

	file_key = g_key_file_new();
	if (!g_key_file_load_from_file (file_key, bench_config_path, G_KEY_FILE_NONE, NULL)){
		ALERT_LOG("READ BENCH CONFIG FILE ERR");
		goto ret_return;
	}
	
	if (0 != get_val(str, file_key, key.c_str(), name.c_str())){
		str.clear();
		goto ret_return;
	}

ret_return:
	if (file_key){
		g_key_file_free(file_key);
	}
	return str;
}

std::string bench_conf_t::get_log_dir() const
{
	return m_log_dir;
}

uint32_t bench_conf_t::get_log_level() const
{
	return m_log_level;
}

uint32_t bench_conf_t::get_log_max_byte() const
{	
	return m_log_max_byte;
}

uint32_t bench_conf_t::get_log_max_files() const
{
	return m_log_max_files;
}

uint32_t bench_conf_t::get_log_save_next_file_interval_min() const
{
	return m_log_save_next_file_interval_min;
}