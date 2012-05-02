#include "lib_include.h"
#include "lib_timer.h"
#include "lib_log.h"

struct timeval  now;
struct tm       tm_cur;

namespace {
#define TIMER_VEC_SIZE    5

	struct tvec_root_s {
		struct list_head head;
		int     expire;
		time_t  min_expiring_time;
		tvec_root_s(){
			INIT_LIST_HEAD(&head);
			expire = 0;
			min_expiring_time = 0;
		}
	};

	struct tvec_root_s  vec[TIMER_VEC_SIZE];
	struct list_head    micro_timer;

	static inline void add_timer(timer_struct_t* t);

	static inline int  find_min_idx(time_t diff, int max_idx);
	static inline void set_min_exptm(time_t exptm, int idx);

	static inline timer_struct_t* find_event(list_head_t* head, timer_cb_func_t func);
	static inline timer_struct_t* find_event_with_expire(list_head_t* head, timer_cb_func_t function, time_t expire);
	void scan_timer_list(const int idx)
	{
		int    i;
		time_t min_exptm = 0;
		timer_struct_t *t;
		list_head_t *cur, *next;

		list_for_each_safe(cur, next, &vec[idx].head) {
			t = list_entry(cur, timer_struct_t, entry);
			if (t->function) {
				i = find_min_idx(t->expire - now.tv_sec, idx);
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

	/*----------------------------------------------
  *  inline utilities
  *----------------------------------------------*/
	inline void add_timer(timer_struct_t *t)
	{
		int i, diff;

		diff = t->expire - now.tv_sec;
		for (i = 0; i != (TIMER_VEC_SIZE - 1); ++i) {
			if (diff <= vec[i].expire)
				break;
		}

		list_add_tail(&t->entry, &vec[i].head);
		set_min_exptm(t->expire, i);
	}

	inline int find_min_idx(time_t diff, int max_idx)
	{
		while (max_idx && (vec[max_idx - 1].expire >= diff)) {
			--max_idx;
		}
		return max_idx;
	}

	inline void set_min_exptm(time_t exptm, int idx)
	{
		if ((exptm < vec[idx].min_expiring_time) || (vec[idx].min_expiring_time == 0)) {
			vec[idx].min_expiring_time = exptm;
		}
	}

	inline timer_struct_t* find_event(list_head_t* head, timer_cb_func_t function)
	{
		timer_struct_t* t;

		list_for_each_entry(t, head, sprite_list) {
			if (t->function == function)
				return t;
		}

		return NULL;
	}

	inline timer_struct_t* find_event_with_expire(list_head_t* head, timer_cb_func_t function, time_t expire)
	{
		timer_struct_t* t;

		list_for_each_entry(t, head, sprite_list) {
			if (t->function == function && t->expire == expire)
				return t;
		}

		return NULL;
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
			timer_struct_t* t = list_entry(l, timer_struct_t, entry);
			do_remove_timer(t, 1);
		}	
	}

	list_for_each_safe(l, p, &micro_timer) {
		micro_timer_struct_t* t = list_entry(l, micro_timer_struct_t, entry);
		remove_micro_timer(t, 1);
	}
}

timer_struct_t* ice::add_event(list_head_t* head, timer_cb_func_t function, void* owner,
						  void* data, time_t expire, E_TIMER_CHG_MODE flag)
{
	timer_struct_t* timer;

	if (flag != timer_add_new_timer) {
		timer = find_event(head, function);
		if (!timer) {
			goto new_timer;
		}

		if ( flag == timer_replace_timer ) {			
			mod_expire_time(timer, expire);
		}
		return timer;
	}
new_timer:
	timer = (timer_struct_t*)g_slice_alloc(sizeof *timer);
	INIT_LIST_HEAD(&timer->sprite_list);
	INIT_LIST_HEAD(&timer->entry);
	timer->function  = function;
	timer->func_indx = 0;
	timer->expire    = expire;
	timer->owner     = owner;
	timer->data      = data;

	list_add_tail(&timer->sprite_list, head);
	add_timer(timer);
	return timer;
}

void ice::scan_seconds_timer()
{
	list_head_t *l, *p;
	timer_struct_t* t;

	list_for_each_safe(l, p, &vec[0].head) {
		t = list_entry(l, timer_struct_t, entry);
		if (!(t->function)) {
			do_remove_timer(t, 1);
		} else if (t->expire <= now.tv_sec) {
			if ( t->function(t->owner, t->data) == 0 ) {
				do_remove_timer(t, 1);
			}
		}
	}

	int i = 1;
	for (; i != TIMER_VEC_SIZE; ++i) {
		if ((vec[i].min_expiring_time - now.tv_sec) < 2) {
			scan_timer_list(i);
		}
	}
}

void ice::mod_expire_time(timer_struct_t* t, time_t expiretime)
{
	if (!t->expire) return;

	time_t diff = expiretime - now.tv_sec;
	int j = 0;

	t->expire = expiretime;
	for ( ; j != (TIMER_VEC_SIZE - 1); ++j ) {
		if ( diff <= vec[j].expire ) break;
	}

	list_del_init(&t->entry);
	list_add_tail(&t->entry, &vec[j].head);
	set_min_exptm(t->expire, j);
}

micro_timer_struct_t* ice::add_micro_event(timer_cb_func_t func, const struct timeval* tv, void* owner, void* data)
{
	micro_timer_struct_t* timer = (micro_timer_struct_t*)g_slice_alloc(sizeof *timer);
	INIT_LIST_HEAD(&timer->entry);
	timer->function  = func;
	timer->func_indx = 0;
	timer->tv        = *tv;
	timer->owner     = owner;
	timer->data      = data;

	list_add_tail(&timer->entry, &micro_timer);
	return timer;
}

void ice::scan_microseconds_timer()
{
	list_head_t *l, *p;
	micro_timer_struct_t* t;

	list_for_each_safe(l, p, &micro_timer) {
		t = list_entry(l, micro_timer_struct_t, entry);
		if (!(t->function)) {
			remove_micro_timer(t, 1);
		} else if (now.tv_sec > t->tv.tv_sec || (now.tv_sec == t->tv.tv_sec && now.tv_usec > t->tv.tv_usec)) {
			if (t->function(t->owner, t->data) == 0) {
				remove_micro_timer(t, 1);
			}
		}
	}
}

void ice::remove_micro_timers(void* owner)
{
	list_head_t *l, *p;
	micro_timer_struct_t* t;

	list_for_each_safe(l, p, &micro_timer) {
		t = list_entry(l, micro_timer_struct_t, entry);
		if (t->owner == owner) {
			remove_micro_timer(t, 0);
		}
	}
}

void ice::handle_timer()
{
	static time_t last = 0;
	renew_now();
	//second timer
	if (last != now.tv_sec) {
		last = now.tv_sec;
		scan_seconds_timer();
	}
	//microseconds timer
	scan_microseconds_timer();
}

void ice::do_remove_timer(timer_struct_t* t, int freed)
{
	if (t->sprite_list.next != 0) {
		list_del(&t->sprite_list);
	}
	if (freed) {
		list_del(&t->entry);
		g_slice_free1(sizeof *t, t);
	} else {
		t->function = 0;
		t->func_indx = 0;
	}
}

void ice::remove_timers(list_head_t* head)
{
	timer_struct_t *t;
	list_head_t *l, *m;

	list_for_each_safe (l, m, head) {
		t = list_entry (l, timer_struct_t, sprite_list);
		do_remove_timer(t, 0);
	}
}

void ice::remove_micro_timer(micro_timer_struct_t *t, int freed)
{
	if (freed) {
		list_del_init(&t->entry);
		g_slice_free1(sizeof *t, t);
	} else {
		t->function = 0;
		t->func_indx = 0;
	}
}
