#include <glib.h>
#include <stdint.h>
#include <sstream>

#include "bench_conf.h"
#include "log.h"

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

	if (0 != get_val(liblogic_path, key, "plugin", "liblogic")){
		ret = -1;
		goto ret;
	}

	if (0 != get_val(max_fd_num, key, "common", "max_fd_num")){
		ret = -1;
		goto ret;
	}

	if (0 != get_val(daemon, key, "common", "is_daemon")){
		ret = -1;
		goto ret;
	}
/*
	std::string s;
	if (0 != get_val(s, key, "log", "dir")){
		ret = -1;
		goto ret;
	}
	s.clear();
	if (0 != get_val(s, key, "log", "level")){
		ret = -1;
		goto ret;
	}
	s.clear();

	if (0 != get_val(s, key, "log", "size")){
		ret = -1;
		goto ret;
	}
	s.clear();

	if (0 != get_val(s, key, "log", "max_files")){
		ret = -1;
		goto ret;
	}
	s.clear();
*/
ret:
	if (key){
		g_key_file_free(key);
	}
	return ret;
}

const char* bench_conf_t::get_liblogic_path() const
{
	return liblogic_path.c_str();
}

uint32_t bench_conf_t::get_max_fd_num() const
{
	return max_fd_num;
}

bench_conf_t::bench_conf_t()
{
	max_fd_num = 0;
	daemon = 0;
}

bool bench_conf_t::is_daemon()
{
	return 1 == daemon;
}