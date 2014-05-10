//
//  BufferFrame.h
//  database
//
//  Created by Jan Michael Auer on 04/05/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#pragma once

#include "utils/Lock.h"
#include "common/IDs.h"

#define BUFFER_FRAME_SIZE 8

namespace lsql {

	/**
	 * The type of queue this frame is currently stored in. This belongs to the 
	 * 2Q page replaement algorithm. See @c BufferManager for more information.
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
		PID id;
		Lock l;
		void* data;
		bool dirty;

	public:

		/**
		 * The size of BufferFrame data arrays. Typically 4096.
		 */
		static size_t SIZE;

		/** Internal pointer for buffer management. */
		BufferFrame* tableNext;

		/** Internal pointer for buffer management. */
		BufferFrame* tablePrev;

		/** Queue type information for page replacement. */
		QueueType queue;

		/** Internal pointer for page replacement. */
		BufferFrame* queueNext;

		/** Internal pointer for page replacement. */
		BufferFrame* queuePrev;
		
		/**
		 * Creates a new buffer frame and allocates enough space to fit
		 * @c BufferFrame::SIZE byte pages. Initially, the frame is not
		 * dirty.
		 *
		 * @param id A unique identifier for this page frame.
		 */
		BufferFrame(const PID& id);

		/** Prevent the copy constructor. */
		BufferFrame(const BufferFrame& other) = delete;

		/** Prevent copy assignments. */
		BufferFrame& operator=(const BufferFrame& other) = delete;

		/**
		 * Creates a new buffer frame and inherits the memory pages of
		 * that frame. The reference to the memory page is removed from 
		 * the old frame.
		 *
		 * This method might fail, if the old frame is dirty and there
		 * is data which has not been written to disk.
		 *
		 * @param id     A unique identifier for this page frame.
		 * @param unused An old buffer frame to take over.
		 */
		BufferFrame(const PID& id, BufferFrame&& unused);

		/**
		 * Destroys this buffer frame and releases all memory allocated
		 * by it. This effectively removes all data stored in this frame.
		 */
		~BufferFrame();

		/**
		 * Returns the id of this frame.
		 */
		const PID& getId() const;

		/**
		 * Returns a pointer to the raw data owned by this frame. Each
		 * modification within this memory segment should result in a
		 * call to @c BufferFrame::setDirty().
		 */
		void* getData();

		/**
		 * Returns whether this frame is dirty or not.
		 */
		bool isDirty() const;

		/**
		 * Marks this page frame as dirty. When the frame is paged out
		 * by the responsible BufferManager, its contents will be written
		 * to disc.
		 *
		 * THIS METHOD SHOULD ONLY BE USED BY THE OWNING @c Buffermanager.
		 */
		void setDirty();

		/**
		 * Reads data of the specified page from disc into the page frame.
		 *
		 * THIS METHOD SHOULD ONLY BE USED BY THE OWNING @c Buffermanager.
		 */
		bool load();

		/**
		 * Writes data from the given page frame to disc. This also resets
		 * the dirty flag of this page frame.
		 *
		 * THIS METHOD SHOULD ONLY BE USED BY THE OWNING @c Buffermanager.
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
		 * THIS METHOD SHOULD ONLY BE USED BY THE OWNING @c Buffermanager.
		 *
		 * @param exclusive Whether or not the lock should be exclusive.
		 * @return True if the lock could be acquired; otherwise false.
		 */
		bool lock(bool exclusive);

		/**
		 * Tries to lock this page.
		 *
		 * If another thread currently holds a conflicting lock on this page,
		 * the method returns false. Conflicts are:
		 *  1. An exclusive lock, if @c exclusive is false.
		 *  2. Any lock mode, if @c exclusive is true.
		 *
		 * Once locked, a page should not be locked again. Upgrading locks from
		 * shared to exclusive is not possible -- instead the current lock has
		 * to be released and a new (exclusive) lock to be acquired.
		 *
		 * THIS METHOD SHOULD ONLY BE USED BY THE OWNING @c Buffermanager.
		 *
		 * @param exclusive Whether or not the lock should be exclusive.
		 * @return True if the lock could be acquired; otherwise false.
		 */
		bool tryLock(bool exclusive);

		/**
		 * Unlocks the previously locked page.
		 *
		 * If this thread holds no more locks on this page, another waiting
		 * thread can acquire the lock. For more information, see @c
		 * pthread_rwlock_unlock().
		 *
		 * THIS METHOD SHOULD ONLY BE USED BY THE OWNING @c Buffermanager.
		 *
		 * @return True if the lock has been released; otherwise false.
		 */
		bool unlock();

	private:

		/**
		 * Detaches this buffer frame from the underlying memory page. 
		 *
		 * After calling this method, the frame can safely be destroyed without the 
		 * underlying memory data to be destroyed. However, if this frame is dirty,
		 * data is not written to disk and might be lost.
		 *
		 * THIS METHOD SHOULD ONLY BE USED BY THE OWNING @c Buffermanager.
		 */
		void invalidate();

	};

}

#include "BufferFrame.cpp"
