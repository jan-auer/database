//
//  BufferFrame.h
//  database
//
//  Created by Jan Michael Auer on 04/05/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#ifndef __database__BufferFrame__
#define __database__BufferFrame__

#include "utils/Lock.h"
#include "PageId.h"

#define BUFFER_FRAME_SIZE 8

namespace lsql {

	/**
	 *
	 */
	enum QueueType {
		QUEUE_NONE = 0,
		QUEUE_A1,
		QUEUE_AM
	};

	/**
	 * Contains information about one page managed by a buffer manager.
	 */
	class BufferFrame {

		// Frame data
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
		 *
		 */
		BufferFrame* tableNext;

		/**
		 *
		 */
		BufferFrame* tablePrev;

		/**
		 *
		 */
		QueueType queue;

		/**
		 *
		 */
		BufferFrame* queueNext;

		/**
		 *
		 */
		BufferFrame* queuePrev;
		
		/**
		 * Creates a new buffer frame and allocates enough space to fit
		 * @c BufferFrame::SIZE byte pages. Initially, the frame is not
		 * dirty.
		 *
		 * @param id A unique identifier for this page frame.
		 */
		BufferFrame(const PageId& id);

		/**
		 * Creates a new buffer frame and inherits the memory pages of
		 * that frame. The unused frame is deleted by this operation and
		 * should already have been removed from all queues and tables.
		 * The given BufferFrame* will be set to @c nullptr.
		 *
		 * @param id     A unique identifier for this page frame.
		 * @param unused An old buffer frame.
		 */
		BufferFrame(const PageId& id, BufferFrame&& unused);

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

		void setData(void* data);

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
		 * Reads data of the specified page from disc into the page frame.
		 */
		bool load();

		/**
		 * Writes data from the given page frame to disc.
		 */
		bool save();

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

		bool tryLock(bool exclusive);

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
