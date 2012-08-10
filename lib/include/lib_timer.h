/********************************************************************
	platform:	
	author:		kevin
	copyright:	All rights reserved.
	purpose:	todo
	brief:		定时器函数，有秒级和微妙级两种精度的接口。用于设定某一时刻调用某个函数。需要glib支持。
*********************************************************************/

#pragma once

#include <glib.h>
#include "lib_include.h"
#include "lib_list.h"

/**
 * @brief   回调函数的类型。如果回调函数返回0，则表示定时器到期时要删除该定时器，反之，则不删除。
 */
typedef int (*ON_TIMER_FUN)(void*, void*);

/**
 * @brief   秒级定时器。
 */
struct lib_timer_sec_t{
	//联系定时器转轮
	struct list_head	entry;
	//联系定时器的拥有者
	struct list_head	sprite_list;
	/*! 定时器的触发时刻，亦即调用回调函数的时间。*/
	time_t				expire;
	void*				owner;
	void*				data;
	ON_TIMER_FUN		function;
	inline void init(){
		INIT_LIST_HEAD(&this->sprite_list);
		INIT_LIST_HEAD(&this->entry);
	}
};

/**
 * @brief   微秒级定时器。
 */
struct lib_timer_micro_t{
	struct list_head	entry;
	/*! 定时器的触发时刻，亦即调用回调函数的时间。*/
	struct timeval		tv;
	void*				owner;
	void*				data;
	ON_TIMER_FUN		function;
	inline void init(){
		INIT_LIST_HEAD(&this->entry);
	}
};

/**
 * @brief  定时器添加模式。用于add_sec_event函数。
 * @see    add_sec_event
 */
enum E_TIMER_CHG_MODE {
	/*! 添加一个新的定时器。*/
	timer_add_new_timer,
	/*! 替换一个定时器。如果找不到符合条件的定时器，则创建一个新的定时器。*/
	timer_replace_timer,
} ;

namespace ice{
	class lib_timer_t{
	public:
		lib_timer_t(){}
		virtual ~lib_timer_t(){}
		/**
		 * @brief  初始化定时器功能。必须调用了这个函数，才能使用定时器功能。
		 * @see    destroy_timer
		 */
		void setup_timer();
		/**
		 * @brief  销毁所有定时器（包括秒级和微秒级的定时器），并释放内存。
		 * @see    setup_timer
		 */
		void destroy_timer();
		/**
		 * @brief  扫描定时器列表，调用到期了的定时器的回调函数，并根据回调函数的返回值决定是否需要把该定时器删除掉。
		 *         如果回调函数返回0，则表示要删除该定时器，反之，则不删除。必须定期调用该函数才能调用到期了的定时器的回调函数。
		 *         注意:调用前一定要调用过renew_now()
		 */
		void handle_timer();

		/**
		 * @brief  添加/替换一个秒级定时器，该定时器的到期时间是expire，到期时回调的函数是func。
		 * @param  head 链头，新创建的定时器会被插入到该链表中。
		 * @param  func 定时器到期时调用的回调函数。
		 * @param  owner 传递给回调函数的第一个参数。
		 * @param  data 传递给回调函数的第二个参数。
		 * @param  expire 定时器到期时间（从Epoch开始的秒数）。
		 * @param  flag 指示add_sec_event添加/替换定时器。如果flag==timer_replace_timer，
		 *         那么add_sec_event将在head链表中搜索出第一个回调函数==func的定时器，
		 *         然后把这个定时器的到期时间修改成expire。如果找不到符合条件的定时器，则新建一个定时器。
		 *         建议只有当head链表中所有定时器的回调函数都各不相同的情况下，才使用timer_replace_timer。
		 *         注意：绝对不能在定时器的回调函数中修改该定时器的到期时间！
		 * @return 指向新添加/替换的秒级定时器的指针。
		 * @see    ADD_TIMER_EVENT, REMOVE_TIMER, remove_sec_timers, REMOVE_TIMERS
		 */
		lib_timer_sec_t* add_sec_event(list_head_t* head, ON_TIMER_FUN func, void* owner, void* data, time_t expire, E_TIMER_CHG_MODE flag);
		/**
		 * @brief  修改秒级定时器tmr的到期时间。注意：绝对不能在定时器的回调函数中修改该定时器的到期时间！
		 * @param  tmr 需要修改到期时间的定时器。
		 * @param  exptm 将tmr的到期时间修改成exptm（从Epoch开始的秒数）。
		 * @see    add_sec_event, ADD_TIMER_EVENT
		 */
		void mod_expire_time(lib_timer_sec_t* tmr, time_t exptm);
		void remove_sec_timer(lib_timer_sec_t* t, int freed = 1);
		/**
		 * @brief  删除链表head中所有的秒级定时器
		 * @param  head 定时器链表的链头。
		 * @see    add_sec_event, ADD_TIMER_EVENT
		 */
		void remove_sec_timers(list_head_t* head);

		/**
		 * @brief  添加一个微秒级定时器，该定时器的到期时间是tv，到期时回调的函数是func。
		 * @param  func 定时器到期时调用的回调函数。
		 * @param  tv 定时器到期时间。
		 * @param  owner 传递给回调函数的第一个参数。
		 * @param  data 传递给回调函数的第二个参数。
		 * @return 指向新添加的微秒级定时器的指针。
		 * @see    REMOVE_MICRO_TIMER, remove_micro_timers, REMOVE_TIMERS
		 */
		lib_timer_micro_t* add_micro_event(ON_TIMER_FUN func, const struct timeval* tv, void* owner, void* data);

		void remove_micro_timer(lib_timer_micro_t *t, int freed = 1);

		/**
		 * @brief  删除传递给回调函数的第一个参数==owner的所有微秒级定时器
		 * @param  owner 传递给回调函数的第一个参数。
		 * @see    add_micro_event, REMOVE_MICRO_TIMER, REMOVE_TIMERS
		 */
		void remove_micro_timers(void* owner);

		/**
		 * @brief 更新当前时间。
		 * @see get_now_tv, get_now_tm
		 */
		static void renew_now();

		/**
		 * @brief 对于对实时性要求不会太高的程序.这样在处理数据包的函数里就可以直接使用get_now_tv来获取不太精确的当前时间，
		 *		从而能稍微提升程序的效率。
		 *		注意:使用前,必须使用renew_now来更新内存中的时间!!!
		 * @return 不太精确的当前时间。
		 * @see renew_now, get_now_tm
		 */
		static const struct timeval* get_now_tv();

		/**
		 * @brief 对于对实时性要求不会太高的程序，这样在处理数据包的函数里就可以直接使用get_now_tm来获取不太精确的当前时间，
		 *		从而能稍微提升程序的效率。
		 *		注意:使用前,必须使用renew_now来更新内存中的时间!!!
		 * @return 不太精确的当前时间。
		 * @see renew_now, get_now_tv
		 */
		static const struct tm* get_now_tm();
	protected:
		
	private:
		lib_timer_t(const lib_timer_t& cr);
		lib_timer_t& operator=(const lib_timer_t& cr);
	};
	
// 	/**
// 	 * @brief  初始化定时器功能。必须调用了这个函数，才能使用定时器功能。
// 	 * @see    destroy_timer
// 	 */
// 	void setup_timer();
// 	/**
// 	 * @brief  销毁所有定时器（包括秒级和微秒级的定时器），并释放内存。
// 	 * @see    setup_timer
// 	 */
// 	void destroy_timer();
// 
// 	/**
// 	 * @brief  扫描定时器列表，调用到期了的定时器的回调函数，并根据回调函数的返回值决定是否需要把该定时器删除掉。
// 	 *         如果回调函数返回0，则表示要删除该定时器，反之，则不删除。必须定期调用该函数才能调用到期了的定时器的回调函数。
// 	 *         注意:调用前一定要调用过renew_now()
// 	 */
// 	void handle_timer();
// 
// 	/**
// 	 * @brief  添加/替换一个秒级定时器，该定时器的到期时间是expire，到期时回调的函数是func。
// 	 * @param  head 链头，新创建的定时器会被插入到该链表中。
// 	 * @param  func 定时器到期时调用的回调函数。
// 	 * @param  owner 传递给回调函数的第一个参数。
// 	 * @param  data 传递给回调函数的第二个参数。
// 	 * @param  expire 定时器到期时间（从Epoch开始的秒数）。
// 	 * @param  flag 指示add_sec_event添加/替换定时器。如果flag==timer_replace_timer，
// 	 *         那么add_sec_event将在head链表中搜索出第一个回调函数==func的定时器，
// 	 *         然后把这个定时器的到期时间修改成expire。如果找不到符合条件的定时器，则新建一个定时器。
// 	 *         建议只有当head链表中所有定时器的回调函数都各不相同的情况下，才使用timer_replace_timer。
// 	 *         注意：绝对不能在定时器的回调函数中修改该定时器的到期时间！
// 	 * @return 指向新添加/替换的秒级定时器的指针。
// 	 * @see    ADD_TIMER_EVENT, REMOVE_TIMER, remove_sec_timers, REMOVE_TIMERS
// 	 */
// 	lib_timer_sec_t* add_sec_event(list_head_t* head, ON_TIMER_FUN func, void* owner, void* data, time_t expire, E_TIMER_CHG_MODE flag);
// 
// 	/**
// 	 * @brief  修改秒级定时器tmr的到期时间。注意：绝对不能在定时器的回调函数中修改该定时器的到期时间！
// 	 * @param  tmr 需要修改到期时间的定时器。
// 	 * @param  exptm 将tmr的到期时间修改成exptm（从Epoch开始的秒数）。
// 	 * @see    add_sec_event, ADD_TIMER_EVENT
// 	 */
// 	void mod_expire_time(lib_timer_sec_t* tmr, time_t exptm);
// 
// 	void remove_sec_timer(lib_timer_sec_t* t, int freed = 1);
// 
// 	/**
// 	 * @brief  删除链表head中所有的秒级定时器
// 	 * @param  head 定时器链表的链头。
// 	 * @see    add_sec_event, ADD_TIMER_EVENT
// 	 */
// 	void remove_sec_timers(list_head_t* head);
// 
// 	/**
// 	 * @brief  添加一个微秒级定时器，该定时器的到期时间是tv，到期时回调的函数是func。
// 	 * @param  func 定时器到期时调用的回调函数。
// 	 * @param  tv 定时器到期时间。
// 	 * @param  owner 传递给回调函数的第一个参数。
// 	 * @param  data 传递给回调函数的第二个参数。
// 	 * @return 指向新添加的微秒级定时器的指针。
// 	 * @see    REMOVE_MICRO_TIMER, remove_micro_timers, REMOVE_TIMERS
// 	 */
// 	lib_timer_micro_t* add_micro_event(ON_TIMER_FUN func, const struct timeval* tv, void* owner, void* data);
// 
// 	void remove_micro_timer(lib_timer_micro_t *t, int freed = 1);
// 
// 	/**
// 	 * @brief  删除传递给回调函数的第一个参数==owner的所有微秒级定时器
// 	 * @param  owner 传递给回调函数的第一个参数。
// 	 * @see    add_micro_event, REMOVE_MICRO_TIMER, REMOVE_TIMERS
// 	 */
// 	void remove_micro_timers(void* owner);
// 
// 	/**
// 	 * @brief 更新当前时间。
// 	 * @see get_now_tv, get_now_tm
// 	 */
// 	void renew_now();
// 
// 	/**
// 	 * @brief 对于对实时性要求不会太高的程序.这样在处理数据包的函数里就可以直接使用get_now_tv来获取不太精确的当前时间，
// 	 *		从而能稍微提升程序的效率。
// 	 *		注意:使用前,必须使用renew_now来更新内存中的时间!!!
// 	 * @return 不太精确的当前时间。
// 	 * @see renew_now, get_now_tm
// 	 */
// 	const struct timeval* get_now_tv();
// 
// 	/**
// 	 * @brief 对于对实时性要求不会太高的程序，这样在处理数据包的函数里就可以直接使用get_now_tm来获取不太精确的当前时间，
// 	 *		从而能稍微提升程序的效率。
// 	 *		注意:使用前,必须使用renew_now来更新内存中的时间!!!
// 	 * @return 不太精确的当前时间。
// 	 * @see renew_now, get_now_tv
// 	 */
// 	const struct tm* get_now_tm();
}//end namespace ice

/**
 * @def    ADD_TIMER_EVENT
 * @brief  创建一个新的秒级定时器，该定时器的到期时间是exptm_，到期时调用的回调函数是func_，传递给回调函数
 *         func_的第一个参数是owner_，第二个参数是data_。
 *         如果你要传递给回调函数的第一个参数是一个结构体，
 *         并且该结构体里面有一个名为timer_list的list_head_t类型的成员变量，那么你就可以使用这个宏来简化创建新定时器的操作。
 * @param  owner_ 传递给回调函数的第一个参数。指针类型，指向一个结构体，并且这个结构体里面必须有一个名为
 *         timer_list的list_head_t类型的成员变量。新创建的定时器会被插入到owner_下的timer_list链表中。
 * @param  func_ 定时器到期时调用的回调函数。
 * @param  data_ 传递给回调函数的第二个参数。
 * @param  exptm_ 定时器的到期时间（从Epoch开始的秒数）。
 * @return 指向新创建的秒级定时器的指针。
 * @see    add_sec_event
 */
#define ADD_TIMER_EVENT(owner_, func_, data_, exptm_) \
	ice::add_sec_event( &((owner_)->timer_list), (func_), (owner_), (data_), (exptm_), timer_add_new_timer)

/**
 * @def    REMOVE_TIMER
 * @brief  删除秒级定时器timer_。
 * @param  timer_ 调用add_sec_event创建定时器时返回的指针。
 * @see    add_sec_event, ADD_TIMER_EVENT, remove_sec_timers
 */
#define REMOVE_TIMER(timer_) \
		ice::remove_sec_timer((timer_), 0)

/**
 * @def    REMOVE_MICRO_TIMER
 * @brief  删除微秒级定时器timer_，并释放内存。
 * @param  timer_ 调用add_micro_event创建定时器时返回的指针。
 * @see    add_micro_event, remove_micro_timers, REMOVE_TIMERS
 */
#define REMOVE_MICRO_TIMER(timer_) \
		ice::remove_micro_timer((timer_), 0)
/**
 * @def    REMOVE_TIMERS
 * @brief  删除owner_指向的结构体中timer_list成员变量中的所有定时器（对应秒级定时器）
 *         删除传递给回调函数的第一个参数==owner_的所有微秒级定时器
 * @param  owner_ 指向一个结构体，并且该结构体里面必须有一个名为timer_list的list_head_t类型的成员变量。
 * @see    ADD_TIMER_EVENT, remove_sec_timers, add_micro_event, remove_micro_timers
 */
#define REMOVE_TIMERS(owner_) \
	ice::remove_sec_timers(&((owner_)->timer_list)), ice::remove_micro_timers((owner_))


