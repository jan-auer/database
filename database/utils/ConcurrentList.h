//
//  ConcurrentList.h
//  database
//
//  Created by Jan Michael Auer on 04/05/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#ifndef __database__ConcurrentList__
#define __database__ConcurrentList__

namespace lsql {

	/**
	 * A threadsafe list implementation. 
	 *
	 * For performance reasons, list items are not owned by this list but 
	 * managed externally. Concurrent access to the list is handled by a global 
	 * read/write lock. Each non constant method tries to acquire an exclusive 
	 * lock. If the lock is held by another thread, the method call blocks until 
	 * the lock is released by the other thread.
	 *
	 * Access to the next and previous elements is handled by the AccessPolicy
	 * template parameter. It must implement the @c ListAccessPolicy interface:
	 *
	 * \code
	 * template<typename Item>
	 * struct ListAccessPolicy {
	 *   Item*& next(Item* item) const;
	 *   Item*& prev(Item* item) const;
	 * };
	 * \endcode
	 *
	 * Be aware that the AccessPolicy must operate on the same item type as the
	 * list. Thus, an element could even be inserted into multiple lists at the 
	 * same time using different policy implementations for each list.
	 */
	template<typename Item, typename AccessPolicy>
	class ConcurrentList : private AccessPolicy {

		using AccessPolicy::prev;
		using AccessPolicy::next;

		Lock l;
		Item* first;
		Item* last;

	public:

		/**
		 * Creates and initializes a new empty list.
		 */
		ConcurrentList();

		/**
		 * Destroys this list instance. 
		 *
		 * In debug mode this method might fail, if there are still elements in
		 * the list.
		 */
		~ConcurrentList();

		/**
		 * Returns a pointer to the first item in this list.
		 *
		 * @return The first item, or @c nullptr if empty.
		 */
		Item* getFirst() const;

		/**
		 * Returns a pointer to the last item in this list.
		 *
		 * @return The last item, or @c nullptr if empty.
		 */
		Item* getLast() const;

		/**
		 * Appends a new item to the end of this list.
		 *
		 * The item must not be in the list. If the list already contains this
		 * item, behavior is undefined.
		 *
		 * @param item A pointer to the new list item.
		 */
		void append(Item* item);

		/**
		 * Prepends a new item to the start of this list.
		 *
		 * The item must not be in the list. If the list already contains this
		 * item, behavior is undefined.
		 *
		 * @param item A pointer to the new list item.
		 */
		void prepend(Item* item);

		/**
		 * Moves an item of this list to the very front.
		 *
		 * The list must contain the given item. If the item is not in this
		 * list, behavior is undefined.
		 *
		 * @param item An item to move to the front.
		 */
		void bringFront(Item* item);

		/**
		 * Removes the given item from the list.
		 *
		 * The item is not deleted by this method. If the list does not contain
		 * the given item, behavior is undefined.
		 *
		 * @param item An item to remove.
		 */
		void remove(Item* item);

		/**
		 * Removes all items from the list.
		 */
		void empty();

		/**
		 * Removes all items from the list and deletes them.
		 */
		void cleanup();

		/**
		 * Locks this list.
		 *
		 * If another thread currently holds a conflicting lock on this list,
		 * the method blocks until the other thread releases its lock.
		 * Conflicts are:
		 *  1. An exclusive lock, if @c exclusive is false.
		 *  2. Any lock mode, if @c exclusive is true.
		 *
		 * Once locked, a list should not be locked again. Upgrading locks from
		 * shared to exclusive is not possible -- instead the current lock has
		 * to be released and a new (exclusive) lock to be acquired.
		 *
		 * @param exclusive Whether or not the lock should be exclusive.
		 * @return True if the lock could be acquired; otherwise false.
		 */
		bool lock(bool exclusive);

		/**
		 * Unlocks the previously locked list.
		 *
		 * If this thread holds no more locks on this list, another waiting
		 * thread can acquire the lock. For more information, see @c
		 * pthread_rwlock_unlock().
		 *
		 * @return True if the lock has been released; otherwise false.
		 */
		bool unlock();

	};

}

#include "ConcurrentList.cpp"

#endif /* defined(__database__ConcurrentList__) */
