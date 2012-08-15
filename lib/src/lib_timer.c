#include "lib_include.h"
#include "lib_timer.h"

struct timeval ice::lib_timer_t::now;
struct tm ice::lib_timer_t::now_tm;

void ice::lib_timer_t::setup_timer()
{
	lib_timer_t::renew_now();
	for (int i = 0; i < E_TIMER_VEC_SIZE; i++) {
		this->vec[i].expire = gen_time_interval(i);
	}
	INIT_LIST_HEAD(&this->micro_timer);
}

void ice::lib_timer_t::destroy_timer()
{
	list_head_t* l;
	list_head_t* p;

	for (int i = 0; i < E_TIMER_VEC_SIZE; i++) {
		list_for_each_safe(l, p, &this->vec[i].head) {
			lib_timer_sec_t* t = list_entry(l, lib_timer_sec_t, entry);
			this->remove_sec_timer(t);
		}	
	}

	list_for_each_safe(l, p, &this->micro_timer) {
		lib_timer_micro_t* t = list_entry(l, lib_timer_micro_t, entry);
		this->remove_micro_timer(t);
	}
}

void ice::lib_timer_t::handle_timer()
{
	static time_t last = 0;

	//second timer
	if (last != lib_timer_t::now.tv_sec) {
		last = lib_timer_t::now.tv_sec;
		scan_sec_timer();
	}
	//microseconds timer
	scan_micro_timer();
}

ice::lib_timer_t::lib_timer_sec_t* ice::lib_timer_t::add_sec_event( list_head_t* head, ON_TIMER_FUN func, void* owner, void* data, time_t expire, E_TIMER_CHG_MODE flag )
{
	lib_timer_sec_t* timer;
	if (E_TIMER_REPLACE_TIMER == flag) {
		timer = find_event(head, func);
		if (NULL == timer){
			goto new_timer;
		}
		this->mod_expire_time(timer, expire);
		return timer;
	}
new_timer:
	timer = (lib_timer_sec_t*)g_slice_alloc(sizeof(lib_timer_sec_t));
	timer->init();
	timer->function  = func;
	timer->expire    = expire;
	timer->owner     = owner;
	timer->data      = data;

	list_add_tail(&timer->sprite_list, head);
	add_timer(timer);
	return timer;
}

void ice::lib_timer_t::mod_expire_time( lib_timer_sec_t* t, time_t expiretime )
{
	t->expire = expiretime;

	int idx = find_root_idx(expiretime);

	list_del_init(&t->entry);
	list_add_tail(&t->entry, &vec[idx].head);
	set_min_exptm(t->expire, idx);
}

void ice::lib_timer_t::remove_sec_timer( lib_timer_sec_t* t, int freed /*= 1*/ )
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

void ice::lib_timer_t::remove_sec_timers( list_head_t* head )
{
	lib_timer_sec_t *t;
	list_head_t *l, *m;

	list_for_each_safe (l, m, head) {
		t = list_entry (l, lib_timer_sec_t, sprite_list);
		this->remove_sec_timer(t, 0);
	}
}

ice::lib_timer_t::lib_timer_micro_t* ice::lib_timer_t::add_micro_event( ON_TIMER_FUN func, const struct timeval* tv, void* owner, void* data )
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

void ice::lib_timer_t::remove_micro_timer( lib_timer_micro_t *t, int freed /*= 1*/ )
{
	if (freed) {
		list_del_init(&t->entry);
		g_slice_free1(sizeof(lib_timer_micro_t), t);
	} else {
		t->function = NULL;
	}
}

void ice::lib_timer_t::remove_micro_timers( void* owner )
{
	list_head_t *l, *p;
	lib_timer_micro_t* t;

	list_for_each_safe(l, p, &micro_timer) {
		t = list_entry(l, lib_timer_micro_t, entry);
		if (t->owner == owner) {
			this->remove_micro_timer(t, 0);
		}
	}
}

void ice::lib_timer_t::renew_sec_timer_list( const int idx )
{
	time_t min_exptm = 0;
	list_head_t* cur;
	list_head_t* next;

	list_for_each_safe(cur, next, &this->vec[idx].head) {
		lib_timer_sec_t* t = list_entry(cur, lib_timer_sec_t, entry);
		if (NULL != t->function) {
			int i = this->find_min_idx(t->expire - lib_timer_t::now.tv_sec, idx);
			if (i != idx) {
				list_del(&t->entry);
				list_add_tail(&t->entry, &this->vec[i].head);
				set_min_exptm(t->expire, i);
			} else if ((t->expire < min_exptm) || 0 == min_exptm) {
				min_exptm = t->expire;
			}
		} else {
			this->remove_sec_timer(t);			
		}
	}

	this->vec[idx].min_expiring_time = min_exptm;
}

void ice::lib_timer_t::scan_micro_timer()
{
	list_head_t *l, *p;
	lib_timer_micro_t* t;

	list_for_each_safe(l, p, &micro_timer) {
		t = list_entry(l, lib_timer_micro_t, entry);
		if (NULL == (t->function)) {
			this->remove_micro_timer(t);
		} else if (lib_timer_t::now.tv_sec > t->tv.tv_sec 
			|| (lib_timer_t::now.tv_sec == t->tv.tv_sec && lib_timer_t::now.tv_usec > t->tv.tv_usec)) {
				if (0 == t->function(t->owner, t->data)) {
					this->remove_micro_timer(t);
				}
		}
	}
}

void ice::lib_timer_t::scan_sec_timer()
{
	list_head_t *l, *p;
	lib_timer_sec_t* t;

	list_for_each_safe(l, p, &vec[0].head) {
		t = list_entry(l, lib_timer_sec_t, entry);
		if (NULL == t->function) {
			this->remove_sec_timer(t);
		} else if (t->expire <= lib_timer_t::now.tv_sec) {
			if (0 == t->function(t->owner, t->data)) {
				this->remove_sec_timer(t);
			}
		}
	}

	for (int i = 1; i != E_TIMER_VEC_SIZE; ++i) {
		if ((this->vec[i].min_expiring_time - lib_timer_t::now.tv_sec) < this->gen_time_interval(i)) {
			this->renew_sec_timer_list(i);
		}
	}
}
