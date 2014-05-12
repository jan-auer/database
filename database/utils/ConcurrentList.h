//
//  ConcurrentList.h
//  database
//
//  Created by Jan Michael Auer on 04/05/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#pragma once

#include <atomic>

#include "Lock.h"

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
		std::atomic<uint64_t> size;

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
		 * @param lock Whether or not to lock the queue.
		 */
		void append(Item* item, bool lock = false);

		/**
		 * Prepends a new item to the start of this list.
		 *
		 * The item must not be in the list. If the list already contains this
		 * item, behavior is undefined.
		 *
		 * @param item A pointer to the new list item.
		 * @param lock Whether or not to lock the queue.
		 */
		void prepend(Item* item, bool lock = false);

		/**
		 * Moves an item of this list to the very front.
		 *
		 * The list must contain the given item. If the item is not in this
		 * list, behavior is undefined.
		 *
		 * @param item An item to move to the front.
		 * @param lock Whether or not to lock the queue.
		 */
		void bringFront(Item* item, bool lock = false);

		/**
		 * Removes the given item from the list.
		 *
		 * The item is not deleted by this method. If the list does not contain
		 * the given item, behavior is undefined.
		 *
		 * @param item An item to remove.
		 * @param lock Whether or not to lock the queue.
		 */
		void remove(Item* item, bool lock = false);

		/**
		 * Removes all items from the list.
		 */
		void empty();

		/**
		 * Removes all items from the list and deletes them.
		 */
		void cleanup();

		/**
		 * Returns the current number of elements in the list.
		 */
		uint64_t getSize() const;

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
		 * Tries to lock this list.
		 *
		 * If another thread currently holds a conflicting lock on this list,
		 * the method returns false. Conflicts are:
		 *  1. An exclusive lock, if @c exclusive is false.
		 *  2. Any lock mode, if @c exclusive is true.
		 *
		 * @param exclusive Whether or not the lock should be exclusive.
		 * @return True if the lock could be acquired; otherwise false.
		 */
		bool tryLock(bool exclusive);

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

#include "ConcurrentList-impl.h"
