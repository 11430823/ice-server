#include "lib_include.h"
#include "lib_timer.h"

namespace {
	#define TIMER_VEC_SIZE    5

	struct tvec_root_s {
		struct list_head head;
		int     expire;
		//最小到期时间
		time_t  min_expiring_time;
		tvec_root_s(){
			INIT_LIST_HEAD(&head);
			expire = 0;
			min_expiring_time = 0;
		}
	};

	struct timeval  now;
	struct tm       tm_cur;
	struct tvec_root_s  vec[TIMER_VEC_SIZE];
	struct list_head    micro_timer;//毫秒是只有一个链表.每次全部检索

	inline int find_min_idx(time_t diff, int max_idx){
		while (max_idx && (vec[max_idx - 1].expire >= diff)) {
			--max_idx;
		}
		return max_idx;
	}

	/**
	* @brief	设置定时轮中一个转轮中到期最小时间
	* @param	time_t exptm 到期时间
	* @param	int idx	定时轮中的序号
	*/
	inline void set_min_exptm(time_t exptm, int idx){
		if ((exptm < vec[idx].min_expiring_time) || (0 == vec[idx].min_expiring_time)) {
			vec[idx].min_expiring_time = exptm;
		}
	}

	void scan_timer_list(const int idx){
		time_t min_exptm = 0;
		list_head_t* cur;
		list_head_t* next;

		list_for_each_safe(cur, next, &vec[idx].head) {
			lib_timer_sec_t* t = list_entry(cur, lib_timer_sec_t, entry);
			if (t->function) {
				int i = find_min_idx(t->expire - now.tv_sec, idx);
				if (i != idx) {
					list_del(&t->entry);
					list_add_tail(&t->entry, &vec[i].head);
					set_min_exptm(t->expire, i);
				} else if ((t->expire < min_exptm) || !min_exptm) {
					min_exptm = t->expire;
				}
			} else {
				ice::do_remove_timer(t, 1);			
			}
		}

		vec[idx].min_expiring_time = min_exptm;
	}

	/**
	* @brief	根据到期时间找时间轮的序号
	* @param	int expire 到期时间
	* @return	int 时间轮序号
	*/
	inline int find_root_idx(int expire){
		int idx = 0;
		int diff = expire - now.tv_sec;
		for (; idx != (TIMER_VEC_SIZE - 1); ++idx) {
			if (diff <= vec[idx].expire){
				break;
			}
		}
		return idx;
	}

	/**
	* @brief	增加一个定时
	*/
	inline void add_timer(lib_timer_sec_t *t){
		int idx = find_root_idx(t->expire);

		list_add_tail(&t->entry, &vec[idx].head);
		set_min_exptm(t->expire, idx);
	}

	/**
	* @brief	找回调函数相同的定时事件
	*/
	inline lib_timer_sec_t* find_event(list_head_t* head, ON_TIMER_FUN function){
		lib_timer_sec_t* t;

		list_for_each_entry(t, head, sprite_list) {
			if (t->function == function){
				return t;
			}
		}

		return NULL;
	}

	inline lib_timer_sec_t* find_event_with_expire(list_head_t* head, ON_TIMER_FUN function, time_t expire){
		lib_timer_sec_t* t;

		list_for_each_entry(t, head, sprite_list) {
			if (t->function == function && t->expire == expire)
				return t;
		}

		return NULL;
	}

	/**
	* @brief	扫描毫秒定时器链表(逐个)
	* @return	void
	*/
	void scan_micro_timer()
	{
		list_head_t *l, *p;
		lib_timer_micro_t* t;

		list_for_each_safe(l, p, &micro_timer) {
			t = list_entry(l, lib_timer_micro_t, entry);
			if (!(t->function)) {
				remove_micro_timer(t, 1);
			} else if (now.tv_sec > t->tv.tv_sec || (now.tv_sec == t->tv.tv_sec && now.tv_usec > t->tv.tv_usec)) {
				if (t->function(t->owner, t->data) == 0) {
					remove_micro_timer(t, 1);
				}
			}
		}
	}

	/**
	* @brief	扫描秒级定时器离链表(转轮序号0)
	* @return	void
	*/
	void scan_sec_timer()
 	{
		list_head_t *l, *p;
		lib_timer_sec_t* t;

		list_for_each_safe(l, p, &vec[0].head) {
			t = list_entry(l, lib_timer_sec_t, entry);
			if (!(t->function)) {
				do_remove_timer(t, 1);
			} else if (t->expire <= now.tv_sec) {
				if ( t->function(t->owner, t->data) == 0 ) {
					do_remove_timer(t, 1);
				}
			}
		}

		for (int i = 1; i != TIMER_VEC_SIZE; ++i) {
			if ((vec[i].min_expiring_time - now.tv_sec) < 2) {//这里判断< (1 << (i+2))????
				scan_timer_list(i);
			}
		}
	}

}//end namespace

void ice::setup_timer()
{
	renew_now();
	for (int i = 0; i < TIMER_VEC_SIZE; i++) {
		vec[i].expire = 1 << (i + 2);
	}
	INIT_LIST_HEAD(&micro_timer);
}

void ice::destroy_timer()
{
	list_head_t *l, *p;

	for (int i = 0; i < TIMER_VEC_SIZE; i++) {
		list_for_each_safe(l, p, &vec[i].head) {
			lib_timer_sec_t* t = list_entry(l, lib_timer_sec_t, entry);
			do_remove_timer(t, 1);
		}	
	}

	list_for_each_safe(l, p, &micro_timer) {
		lib_timer_micro_t* t = list_entry(l, lib_timer_micro_t, entry);
		remove_micro_timer(t, 1);
	}
}

lib_timer_sec_t* ice::add_sec_event(list_head_t* head, ON_TIMER_FUN function, void* owner,
						  void* data, time_t expire, E_TIMER_CHG_MODE flag)
{
	lib_timer_sec_t* timer;

	if (timer_add_new_timer != flag) {
		timer = find_event(head, function);
		if (NULL == timer){
			goto new_timer;
		}

		if ( timer_replace_timer == flag){			
			mod_expire_time(timer, expire);
		}
		return timer;
	}
new_timer:
	timer = (lib_timer_sec_t*)g_slice_alloc(sizeof(lib_timer_sec_t));
	timer->init();
	timer->function  = function;
	timer->expire    = expire;
	timer->owner     = owner;
	timer->data      = data;

	list_add_tail(&timer->sprite_list, head);
	add_timer(timer);
	return timer;
}

void ice::mod_expire_time(lib_timer_sec_t* t, time_t expiretime)
{
	if (!t->expire) return;

	t->expire = expiretime;

	int idx = find_root_idx(expiretime);

	list_del_init(&t->entry);
	list_add_tail(&t->entry, &vec[idx].head);
	set_min_exptm(t->expire, idx);
}

lib_timer_micro_t* ice::add_micro_event(ON_TIMER_FUN func, const struct timeval* tv, void* owner, void* data)
{
	lib_timer_micro_t* timer = (lib_timer_micro_t*)g_slice_alloc(sizeof(lib_timer_micro_t));
	timer->init();
	timer->function  = func;
	timer->tv        = *tv;
	timer->owner     = owner;
	timer->data      = data;

	list_add_tail(&timer->entry, &micro_timer);
	return timer;
}



void ice::remove_micro_timers(void* owner)
{
	list_head_t *l, *p;
	lib_timer_micro_t* t;

	list_for_each_safe(l, p, &micro_timer) {
		t = list_entry(l, lib_timer_micro_t, entry);
		if (t->owner == owner) {
			remove_micro_timer(t, 0);
		}
	}
}

void ice::renew_now()
{
	gettimeofday(&now, 0);
	localtime_r(&now.tv_sec, &tm_cur);
}

const struct timeval* ice::get_now_tv()
{
	return &now;
}

const struct tm* ice::get_now_tm()
{
	return &tm_cur;
}


void ice::handle_timer()
{
	static time_t last = 0;

	//second timer
	if (last != now.tv_sec) {
		last = now.tv_sec;
		scan_sec_timer();
	}
	//microseconds timer
	scan_micro_timer();
}

void ice::do_remove_timer(lib_timer_sec_t* t, int freed)
{
	if (t->sprite_list.next != 0) {
		list_del(&t->sprite_list);
	}
	if (freed) {
		list_del(&t->entry);
		g_slice_free1(sizeof *t, t);
	} else {
		t->function = 0;
	}
}

void ice::remove_timers(list_head_t* head)
{
	lib_timer_sec_t *t;
	list_head_t *l, *m;

	list_for_each_safe (l, m, head) {
		t = list_entry (l, lib_timer_sec_t, sprite_list);
		do_remove_timer(t, 0);
	}
}

void ice::remove_micro_timer(lib_timer_micro_t *t, int freed)
{
	if (freed) {
		list_del_init(&t->entry);
		g_slice_free1(sizeof(lib_timer_micro_t), t);
	} else {
		t->function = 0;
	}
}

