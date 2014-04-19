//
//  ConcurrentMap.h
//  database
//
//  Created by Jan Michael Auer on 17/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#ifndef __database__ConcurrentMap__
#define __database__ConcurrentMap__

#include <sys/types.h>
#include "Locking.h"

namespace lsql {
	
	/**
	 * A key value store with support for concurrency.
	 *
	 * Concurrency is accomplished by read/write locks on entries within the
	 * map. The user might control those entry locks using @c lsql::lock and
	 * @lsql::unlock on the supplied type lsql::Lock. When reading from and
	 * writing to the map, an optional @c Lock* parameter is accepted. If,
	 * and only if, this parameter is supplied, it is ensured that no other 
	 * thread can access the requested entry between the method call and the
	 * release of the respective lock using @c lsql::unlock.
	 *
	 * \code
	 * lsql::Lock* lock;
	 * lsql::ConcurrentMap<int, int> map(10);
	 * map.insert(42, 42, lock);  // begin of critical area
	 * lsql::unlock(lock);        // end of critical area
	 * \endcode
	 * 
	 * @param Key   The type of the key. It should be integer or implicitly
	 *              convertible to an integer type.
	 * @param Value The type of values to store in the map. Due to performance
	 *              reasons the value type must contain the key type as first
	 *              field (i.e. @c Value can be cast to @c Key).
	 */
	template<typename Key, typename Value>
	class ConcurrentMap {
		
	public:
		
		/// Data structure for map entries.
		/// This needs to be public because it is used as return value.
		/// TODO: Should a key be stored to remove the Key<>Value dependency?
		struct Entry {
			Lock lock = PTHREAD_RWLOCK_INITIALIZER;
			Value value;
			Entry* next;
			
			Entry(const Value& value, Entry* next = nullptr) : value(value), next(next) {}
			~Entry() { pthread_rwlock_destroy(&lock); }
		};

		/// Data structure for hashtable slots.
		/// This needs to be public because it is used as return value.
		/// TODO: Streamline with Entry?
		struct Slot {
			Lock lock = PTHREAD_RWLOCK_INITIALIZER;
			Entry* next = nullptr;
			~Slot() { pthread_rwlock_destroy(&lock); }
		};
		
	private:
		
		Slot* slots;
		off_t slotCount;
		
	public:
		
		/**
		 * Creates a new @c ConcurrentMap instance.
		 *
		 * The internal size of the map is guaranteed to be the smallest
		 * power of 2 greater than @c size. This allows hash functions 
		 * to use binary operations and bit shifts instead of modulo.
		 *
		 * @param size The approximate number of elements to be stored
		 *             in this map. Be aware that this map will not grow
		 *             based on the number of stored elements.
		 */
		ConcurrentMap(off_t size);
		
		/**
		 * Destorys this map and deletes all remaining values, if any.
		 */
		~ConcurrentMap();
		
		/**
		 * Checks if the map contains an entry with the specified key. 
		 *
		 * In case parameter @c lock is supplied, the respective entry
		 * will be charged with a shared lock. The lock can be removed
		 * using @c lsql::unlock().
		 *
		 * If another thread currently holds an exclusive lock on the
		 * desired entry, this functions blocks until the other thread
		 * releases its lock.
		 * 
		 * @param key  The key of the entry.
		 * @param lock Reference to a pointer which receives the lock.
		 *
		 * @return True, if the entry is in the map; otherwise false.
		 */
		bool contains(const Key& key, Lock** lock = nullptr);
		
		/**
		 * Fetches an entry from the map, if possible.
		 *
		 * In case parameter @c lock is supplied, the respective entry
		 * will be charged with a lock based on @c exclusive. The lock 
		 * can be removed using @c lsql::unlock(). A @c lock must be 
		 * supplied, if @c exclusive is true.
		 *
		 * If another thread currently holds a conflicting lock on the
		 * desired entry, this functions blocks until the other thread
		 * releases its lock. Conflicts are:
		 *  1. An exclusive lock, if @c exclusive is false.
		 *  2. Any lock, if @c exclusive is true.
		 *
		 * @param key       The key of the entry.
		 * @param lock      Reference to a pointer which receives the lock.
		 * @param exclusive Whether the lock should be exclusive or not.
		 *
		 * @return A pointer to the entry, if it is in the map;
		 *         otherwise @c nullptr.
		 */
		Value* get(const Key& key, Lock** lock = nullptr, bool exclusive = false);
		
		/**
		 * Inserts a new entry into the map. 
		 *
		 * In case parameter @c lock is supplied, the created entry
		 * will be charged with an exclusive lock. The lock should be
		 * removed using @c lsql::unlock() after the value has been
		 * completely constructed. This might be useful, if building the
		 * value is rather slow, but a reference can be obtained quickly.
		 *
		 * @param key   The key of the new entry. It must match the first
		 *              field of the value.
		 * @param value The value to copy into the map.
		 * @param lock  Reference to a pointer which receives the lock.
		 */
		void insert(const Key& key, const Value& value, Lock** lock = nullptr);
		
		/**
		 * Removes an entry from the map, if possible. 
		 * 
		 * If another thread currently holds any lock on the desired entry, 
		 * this functions blocks until the other thread releases all its 
		 * locks.
		 *
		 * @param key The key of the entry to remove.
		 */
		void remove(const Key& key);
		
	private:
		
		/**
		 * Hashes the given key using an internal hashing algorithm. 
		 * The hash is guaranteed not to exceed @c slotCount.
		 *
		 * @param key A numeric key to obtain the hash value from.
		 * @return The hashed value of @c key.
		 */
		inline off_t hash(const Key& key) const;
		
		/**
		 * Returns the proper slot for an entry with the specified
		 * key. The @c hash() method is used to determine the slot
		 * number. 
		 * 
		 * @param key A numeric key.
		 * @return A reference to the proper slot.
		 */
		Slot& getSlot(const Key& key);
				
		/**
		 * Resolves the reference to a pointer of an element.
		 * 
		 * Using this reference, the caller can access the element 
		 * or change the pointer to either insert a new element or
		 * remove the old one. 
		 *
		 * In case parameter @c lock is supplied, the respective entry
		 * will be charged with a lock based on @c exclusive. The lock
		 * can be removed using @c lsql::unlock(). A @c lock must be
		 * supplied, if @c exclusive is true.
		 *
		 * If another thread currently holds a conflicting lock on the
		 * desired entry, this functions blocks until the other thread
		 * releases its lock. Conflicts are:
		 *  1. An exclusive lock, if @c exclusive is false.
		 *  2. Any lock, if @c exclusive is true.
		 *
		 * @param slot      A slot which is expected to be locked with
		 *                  at least a shared lock. The slot should be
		 *                  exclusively locket if the entry pointer will
		 *                  be modified.
		 * @param key       The key of the entry to find.
		 * @param lock      Reference to a pointer which receives the lock
		 * @param exclusive Whether the entry lock should be exclusive or not.
		 */
		Entry** findRef(Slot& slot, const Key& key, Lock** lock, bool exclusive);
		
	};

}

#include "ConcurrentMap.cpp"

#endif /* defined(__database__ConcurrentMap__) */
