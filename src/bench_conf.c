#include <glib.h>

#include "bench_conf.h"
#include "log.h"

bench_conf_t bench_conf;

namespace {

	const char* bench_config_path = "./bench.ini";

	//************************************
	// Brief:     获取指定组中key的值
	// Returns:   int(0:成功.其它:失败)
	// Parameter: std::string & data(传出值)
	//************************************
	int get_val(std::string& data, GKeyFile* keyfile,
		const char* group_name, const char* key_name)
	{
		gchar **a = NULL;
		gsize len = 0;

		if (NULL == (a = g_key_file_get_string_list (keyfile, 
			group_name, key_name, &len, NULL))){
			ERROR_LOG("read bench config file key err [group_name:%s, key_name:%s]",
				group_name, key_name);
			return -1;  
		}else{
			for (int i = 0; i < len; i++){
				data += a[i];
			}
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
		ERROR_LOG("read bench config file exists err");
		ret = -1;
		goto ret;
	}

	key = g_key_file_new();
	if (!g_key_file_load_from_file (key, bench_config_path, G_KEY_FILE_NONE, NULL)){
		ERROR_LOG("read bench config file err");
		ret = -1;
		goto ret;
	}

	//读取数据
	if (0 != get_val(liblogic_path, key, "plugin", "liblogic")){
		ret = -1;
		goto ret;
	}

ret:
	if (key){
		g_key_file_free(key);
	}
	return ret;
}

std::string& bench_conf_t::get_liblogic_path()
{
	return liblogic_path;
}
