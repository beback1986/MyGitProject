/**
 * 
 * I grub it from linux kernel source code and fix it for user space
 * program. Of course, this is a GPL licensed header file.
 *
 * Here is a recipe to cook list.h for user space program
 *
 * 1. copy list.h from linux/include/list.h
 * 2. remove 
 *     - #ifdef __KERNE__ and its #endif
 *     - all #include line
 *     - prefetch() and rcu related functions
 * 3. add macro bw_offsetof() and bw_container_of
 *
 * - kazutomo@mcs.anl.gov
 */
#ifndef __BWOSL_LIST_H__
#define __BWOSL_LIST_H__

#ifndef NULL
#define NULL 0
#endif

/**
 * @name from other kernel headers
 */
/*@{*/

/**
 * Get offset of a member
 */
#define bw_offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

/**
 * Casts a member of a structure out to the containing structure
 * @param ptr        the pointer to the member.
 * @param type       the type of the container struct this is embedded in.
 * @param member     the name of the member within the struct.
 *
 */
#define bw_container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - bw_offsetof(type,member) );})
/*@}*/


/*
 * These are non-NULL pointers that will result in page faults
 * under normal circumstances, used to verify that nobody uses
 * non-initialized list entries.
 */
#define BW_LIST_POISON1  ((void *) 0x00100100)
#define BW_LIST_POISON2  ((void *) 0x00200200)

/**
 * Simple doubly linked list implementation.
 *
 * Some of the internal functions ("__xxx") are useful when
 * manipulating whole lists rather than single entries, as
 * sometimes we already know the next/prev entries and we can
 * generate better code by using them directly rather than
 * using the generic single-entry routines.
 */
struct bw_list_head {
	struct bw_list_head *next, *prev;
};

#define BW_LIST_HEAD_INIT(name) { &(name), &(name) }

#define BW_LIST_HEAD(name) \
	struct bw_list_head name = BW_LIST_HEAD_INIT(name)

#define BW_INIT_LIST_HEAD(ptr) do { \
	(ptr)->next = (ptr); (ptr)->prev = (ptr); \
} while (0)

/*
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __bw_list_add(struct bw_list_head *new,
			      struct bw_list_head *prev,
			      struct bw_list_head *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

/**
 * bw_list_add - add a new entry
 * @new: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
static inline void bw_list_add(struct bw_list_head *new, struct bw_list_head *head)
{
	__bw_list_add(new, head, head->next);
}

/**
 * bw_list_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
static inline void bw_list_add_tail(struct bw_list_head *new, struct bw_list_head *head)
{
	__bw_list_add(new, head->prev, head);
}


/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __bw_list_del(struct bw_list_head * prev, struct bw_list_head * next)
{
	next->prev = prev;
	prev->next = next;
}

/**
 * bw_list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: bw_list_empty on entry does not return true after this, the entry is
 * in an undefined state.
 */
static inline void bw_list_del(struct bw_list_head *entry)
{
	__bw_list_del(entry->prev, entry->next);
	entry->next = BW_LIST_POISON1;
	entry->prev = BW_LIST_POISON2;
}



/**
 * bw_list_del_init - deletes entry from list and reinitialize it.
 * @entry: the element to delete from the list.
 */
static inline void bw_list_del_init(struct bw_list_head *entry)
{
	__bw_list_del(entry->prev, entry->next);
	BW_INIT_LIST_HEAD(entry);
}

/**
 * bw_list_move - delete from one list and add as another's head
 * @list: the entry to move
 * @head: the head that will precede our entry
 */
static inline void bw_list_move(struct bw_list_head *list, struct bw_list_head *head)
{
        __bw_list_del(list->prev, list->next);
        bw_list_add(list, head);
}

/**
 * bw_list_move_tail - delete from one list and add as another's tail
 * @list: the entry to move
 * @head: the head that will follow our entry
 */
static inline void bw_list_move_tail(struct bw_list_head *list,
				  struct bw_list_head *head)
{
        __bw_list_del(list->prev, list->next);
        bw_list_add_tail(list, head);
}

/**
 * bw_list_empty - tests whether a list is empty
 * @head: the list to test.
 */
static inline int bw_list_empty(const struct bw_list_head *head)
{
	return head->next == head;
}

static inline void __bw_list_splice(struct bw_list_head *list,
				 struct bw_list_head *head)
{
	struct bw_list_head *first = list->next;
	struct bw_list_head *last = list->prev;
	struct bw_list_head *at = head->next;

	first->prev = head;
	head->next = first;

	last->next = at;
	at->prev = last;
}

/**
 * bw_list_splice - join two lists
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
static inline void bw_list_splice(struct bw_list_head *list, struct bw_list_head *head)
{
	if (!bw_list_empty(list))
		__bw_list_splice(list, head);
}

/**
 * bw_list_splice_init - join two lists and reinitialise the emptied list.
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * The list at @list is reinitialised
 */
static inline void bw_list_splice_init(struct bw_list_head *list,
				    struct bw_list_head *head)
{
	if (!bw_list_empty(list)) {
		__bw_list_splice(list, head);
		BW_INIT_LIST_HEAD(list);
	}
}

/**
 * bw_list_entry - get the struct for this entry
 * @ptr:	the &struct bw_list_head pointer.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_struct within the struct.
 */
#define bw_list_entry(ptr, type, member) \
	bw_container_of(ptr, type, member)

/**
 * bw_list_for_each	-	iterate over a list
 * @pos:	the &struct bw_list_head to use as a loop counter.
 * @head:	the head for your list.
 */

#define bw_list_for_each(pos, head) \
  for (pos = (head)->next; pos != (head);	\
       pos = pos->next)

/**
 * __bw_list_for_each	-	iterate over a list
 * @pos:	the &struct bw_list_head to use as a loop counter.
 * @head:	the head for your list.
 *
 * This variant differs from bw_list_for_each() in that it's the
 * simplest possible list iteration code, no prefetching is done.
 * Use this for code that knows the list to be very short (empty
 * or 1 entry) most of the time.
 */
#define __bw_list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * bw_list_for_each_prev	-	iterate over a list backwards
 * @pos:	the &struct bw_list_head to use as a loop counter.
 * @head:	the head for your list.
 */
#define bw_list_for_each_prev(pos, head) \
	for (pos = (head)->prev; prefetch(pos->prev), pos != (head); \
        	pos = pos->prev)

/**
 * bw_list_for_each_safe	-	iterate over a list safe against removal of list entry
 * @pos:	the &struct bw_list_head to use as a loop counter.
 * @n:		another &struct bw_list_head to use as temporary storage
 * @head:	the head for your list.
 */
#define bw_list_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); \
		pos = n, n = pos->next)

/**
 * bw_list_for_each_entry	-	iterate over list of given type
 * @pos:	the type * to use as a loop counter.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define bw_list_for_each_entry(pos, head, member)				\
	for (pos = bw_list_entry((head)->next, typeof(*pos), member);	\
	     &pos->member != (head);					\
	     pos = bw_list_entry(pos->member.next, typeof(*pos), member))

/**
 * bw_list_for_each_entry_reverse - iterate backwards over list of given type.
 * @pos:	the type * to use as a loop counter.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define bw_list_for_each_entry_reverse(pos, head, member)			\
	for (pos = bw_list_entry((head)->prev, typeof(*pos), member);	\
	     &pos->member != (head); 	\
	     pos = bw_list_entry(pos->member.prev, typeof(*pos), member))

/**
 * bw_list_prepare_entry - prepare a pos entry for use as a start point in
 *			bw_list_for_each_entry_continue
 * @pos:	the type * to use as a start point
 * @head:	the head of the list
 * @member:	the name of the list_struct within the struct.
 */
#define bw_list_prepare_entry(pos, head, member) \
	((pos) ? : bw_list_entry(head, typeof(*pos), member))

/**
 * bw_list_for_each_entry_continue -	iterate over list of given type
 *			continuing after existing point
 * @pos:	the type * to use as a loop counter.
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define bw_list_for_each_entry_continue(pos, head, member) 		\
	for (pos = bw_list_entry(pos->member.next, typeof(*pos), member);	\
	     &pos->member != (head);	\
	     pos = bw_list_entry(pos->member.next, typeof(*pos), member))

/**
 * bw_list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:	the type * to use as a loop counter.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define bw_list_for_each_entry_safe(pos, n, head, member)			\
	for (pos = bw_list_entry((head)->next, typeof(*pos), member),	\
		n = bw_list_entry(pos->member.next, typeof(*pos), member);	\
	     &pos->member != (head); 					\
	     pos = n, n = bw_list_entry(n->member.next, typeof(*n), member))

/**
 * bw_list_for_each_entry_safe_continue -	iterate over list of given type
 *			continuing after existing point safe against removal of list entry
 * @pos:	the type * to use as a loop counter.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define bw_list_for_each_entry_safe_continue(pos, n, head, member) 		\
	for (pos = bw_list_entry(pos->member.next, typeof(*pos), member), 		\
		n = bw_list_entry(pos->member.next, typeof(*pos), member);		\
	     &pos->member != (head);						\
	     pos = n, n = bw_list_entry(n->member.next, typeof(*n), member))

/**
 * bw_list_for_each_entry_safe_reverse - iterate backwards over list of given type safe against
 *				      removal of list entry
 * @pos:	the type * to use as a loop counter.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define bw_list_for_each_entry_safe_reverse(pos, n, head, member)		\
	for (pos = bw_list_entry((head)->prev, typeof(*pos), member),	\
		n = bw_list_entry(pos->member.prev, typeof(*pos), member);	\
	     &pos->member != (head); 					\
	     pos = n, n = bw_list_entry(n->member.prev, typeof(*n), member))




/*
 * Double linked lists with a single pointer list head.
 * Mostly useful for hash tables where the two pointer list head is
 * too wasteful.
 * You lose the ability to access the tail in O(1).
 */

struct bw_hlist_head {
	struct bw_hlist_node *first;
};

struct bw_hlist_node {
	struct bw_hlist_node *next, **pprev;
};

#define BW_HLIST_HEAD_INIT { .first = NULL }
#define BW_HLIST_HEAD(name) struct bw_hlist_head name = {  .first = NULL }
#define BW_INIT_HLIST_HEAD(ptr) ((ptr)->first = NULL)
#define BW_INIT_HLIST_NODE(ptr) ((ptr)->next = NULL, (ptr)->pprev = NULL)

static inline int bw_hlist_unhashed(const struct bw_hlist_node *h)
{
	return !h->pprev;
}

static inline int bw_hlist_empty(const struct bw_hlist_head *h)
{
	return !h->first;
}

static inline void __bw_hlist_del(struct bw_hlist_node *n)
{
	struct bw_hlist_node *next = n->next;
	struct bw_hlist_node **pprev = n->pprev;
	*pprev = next;
	if (next)
		next->pprev = pprev;
}

static inline void bw_hlist_del(struct bw_hlist_node *n)
{
	__bw_hlist_del(n);
	n->next = BW_LIST_POISON1;
	n->pprev = BW_LIST_POISON2;
}


static inline void bw_hlist_del_init(struct bw_hlist_node *n)
{
	if (n->pprev)  {
		__bw_hlist_del(n);
		BW_INIT_HLIST_NODE(n);
	}
}

static inline void bw_hlist_add_head(struct bw_hlist_node *n, struct bw_hlist_head *h)
{
	struct bw_hlist_node *first = h->first;
	n->next = first;
	if (first)
		first->pprev = &n->next;
	h->first = n;
	n->pprev = &h->first;
}



/* next must be != NULL */
static inline void bw_hlist_add_before(struct bw_hlist_node *n,
					struct bw_hlist_node *next)
{
	n->pprev = next->pprev;
	n->next = next;
	next->pprev = &n->next;
	*(n->pprev) = n;
}

static inline void bw_hlist_add_after(struct bw_hlist_node *n,
					struct bw_hlist_node *next)
{
	next->next = n->next;
	n->next = next;
	next->pprev = &n->next;

	if(next->next)
		next->next->pprev  = &next->next;
}



#define bw_hlist_entry(ptr, type, member) bw_container_of(ptr,type,member)

#define bw_hlist_for_each(pos, head) \
	for (pos = (head)->first; pos && ({ prefetch(pos->next); 1; }); \
	     pos = pos->next)

#define bw_hlist_for_each_safe(pos, n, head) \
	for (pos = (head)->first; pos && ({ n = pos->next; 1; }); \
	     pos = n)

/**
 * bw_hlist_for_each_entry	- iterate over list of given type
 * @tpos:	the type * to use as a loop counter.
 * @pos:	the &struct bw_hlist_node to use as a loop counter.
 * @head:	the head for your list.
 * @member:	the name of the bw_hlist_node within the struct.
 */
#define bw_hlist_for_each_entry(tpos, pos, head, member)			 \
	for (pos = (head)->first;					 \
	     pos && ({ prefetch(pos->next); 1;}) &&			 \
		({ tpos = bw_hlist_entry(pos, typeof(*tpos), member); 1;}); \
	     pos = pos->next)

/**
 * bw_hlist_for_each_entry_continue - iterate over a hlist continuing after existing point
 * @tpos:	the type * to use as a loop counter.
 * @pos:	the &struct bw_hlist_node to use as a loop counter.
 * @member:	the name of the bw_hlist_node within the struct.
 */
#define bw_hlist_for_each_entry_continue(tpos, pos, member)		 \
	for (pos = (pos)->next;						 \
	     pos && ({ prefetch(pos->next); 1;}) &&			 \
		({ tpos = bw_hlist_entry(pos, typeof(*tpos), member); 1;}); \
	     pos = pos->next)

/**
 * bw_hlist_for_each_entry_from - iterate over a hlist continuing from existing point
 * @tpos:	the type * to use as a loop counter.
 * @pos:	the &struct bw_hlist_node to use as a loop counter.
 * @member:	the name of the bw_hlist_node within the struct.
 */
#define bw_hlist_for_each_entry_from(tpos, pos, member)			 \
	for (; pos && ({ prefetch(pos->next); 1;}) &&			 \
		({ tpos = bw_hlist_entry(pos, typeof(*tpos), member); 1;}); \
	     pos = pos->next)

/**
 * bw_hlist_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @tpos:	the type * to use as a loop counter.
 * @pos:	the &struct bw_hlist_node to use as a loop counter.
 * @n:		another &struct bw_hlist_node to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the bw_hlist_node within the struct.
 */
#define bw_hlist_for_each_entry_safe(tpos, pos, n, head, member) 		 \
	for (pos = (head)->first;					 \
	     pos && ({ n = pos->next; 1; }) && 				 \
		({ tpos = bw_hlist_entry(pos, typeof(*tpos), member); 1;}); \
	     pos = n)


#endif /* __BWOSL_LIST_H__ */
