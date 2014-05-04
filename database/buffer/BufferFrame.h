//
//  BufferFrame.h
//  database
//
//  Created by Jan Michael Auer on 17/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#ifndef __database__BufferFrame__
#define __database__BufferFrame__

#include <cstdint>
#include <sys/types.h>

#include "utils/Lock.h"
#include "PageId.h"

namespace lsql {

	/**
	 * Contains information about one page managed by a buffer manager.
	 */
	class BufferFrame {
		
		PageId id;
		Lock l;
		void* data;
		bool dirty;
		
	public:
		
		/**
		 * The size of BufferFrame data arrays. Typically 4096.
		 */
		static size_t SIZE;
		
		/**
		 * Creates a new buffer frame and allocates enough space to fit
		 * @c BufferFrame::SIZE byte pages. Initially, the frame is not
		 * dirty.
		 *
		 * @param id A unique identifier for this page frame.
		 */
		BufferFrame(const PageId& id);
		
		/**
		 * Destroys this buffer frame and releases all memory allocated
		 * by it. This effectively removes all data stored in this frame.
		 */
		~BufferFrame();
		
		/**
		 * Returns the id of this frame.
		 */
		const PageId& getId() const;

		/**
		 * Returns a pointer to the raw data owned by this frame. Each
		 * modification within this memory segment should result in a
		 * call to @c BufferFrame::setDirty().
		 */
		void* getData();
		
		/**
		 * Returns whether this frame is dirty or not. By default, the
		 * frame is not dirty.
		 */
		bool isDirty() const;
		
		/**
		 * Marks this page frame as dirty. When the frame is paged out
		 * by the responsible BufferManager, its contents will be written
		 * to disc.
		 */
		void setDirty();

		/**
		 * Locks this page.
		 *
		 * If another thread currently holds a conflicting lock on this page,
		 * the method blocks until the other thread releases its lock. 
		 * Conflicts are:
		 *  1. An exclusive lock, if @c exclusive is false.
		 *  2. Any lock mode, if @c exclusive is true.
		 *
		 * Once locked, a page should not be locked again. Upgrading locks from
		 * shared to exclusive is not possible -- instead the current lock has
		 * to be released and a new (exclusive) lock to be acquired. 
		 *
		 * @param exclusive Whether or not the lock should be exclusive.
		 * @return True if the lock could be acquired; otherwise false.
		 */
		bool lock(bool exclusive);

		/**
		 * Unlocks the previously locked page.
		 *
		 * If this thread holds no more locks on this page, another waiting 
		 * thread can acquire the lock. For more information, see @c
		 * pthread_rwlock_unlock().
		 *
		 * @return True if the lock has been released; otherwise false.
		 */
		bool unlock();
		
	};
	
}

#include "BufferFrame.cpp"

#endif /* defined(__database__BufferFrame__) */
