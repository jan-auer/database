//
//  BufferManager.h
//  database
//
//  Created by Jan Michael Auer on 04/05/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#pragma once

#include <atomic>

#include "utils/ConcurrentList.h"
#include "utils/Mutex.h"
#include "common/IDs.h"
#include "BufferFrame.h"

#define BUFFER_MANAGER_QUEUE_THRESHOLD 0.5
#define EXIT_CODE_NO_MORE_MEMORY 42

namespace lsql {

	/**
	 * Manages buffer frames and controls concurrent access to these frames.
	 * Frames are loaded from and stored into a file on the disc. For more
	 * information about frames, see @c BufferFrame.
	 */
	class BufferManager {

		/**
		 * Implementation of the ConcurrentList AccessPolicy for Queues.
		 */
		struct QueuePolicy {
			BufferFrame*& next(BufferFrame* frame) const { return frame->queueNext; }
			BufferFrame*& prev(BufferFrame* frame) const { return frame->queuePrev; }
		};

		/**
		 * Implementation of the ConcurrentList AccessPolicy for the page table.
		 */
		struct TablePolicy {
			BufferFrame*& next(BufferFrame* frame) const { return frame->tableNext; }
			BufferFrame*& prev(BufferFrame* frame) const { return frame->tablePrev; }
		};
		
		/**
		 * Type definition for threadsafe queues of buffer frames.
		 */
		typedef ConcurrentList<BufferFrame, QueuePolicy> Queue;

		/**
		 * Type definition for threadsafe lists of buffer frames in page tables.
		 */
		typedef ConcurrentList<BufferFrame, TablePolicy> Slot;

		Mutex mutex;
		uint64_t freePages;
		uint64_t queueThreshold;
		uint64_t slotCount;

		Slot* pageTable;
		Queue queueA1;
		Queue queueAm;

	public:

		/**
		 * Creates a new buffer manager instance which operates on the
		 * specified file.
		 *
		 * @param size The maximum number of frames in memory.
		 */
		BufferManager(uint64_t size);

		/**
		 * Destroys this buffer manager instance and writes all dirty
		 * frames to disc.
		 */
		~BufferManager();

		/**
		 * Retrieves the specified frame. If the frame is not in memory,
		 * it will be loaded from disc. This might force old unfixed
		 * frames to get flushed to disc and removed from memory.
		 *
		 * If there is no space left for new frames, this method might
		 * fail to return a frame and will exit with an error message.
		 *
		 * @param id        An identifier for the page to load.
		 * @param exclusive Whether or not this frame is exclusive to the
		 *                  caller of this method.
		 *
		 * @return A reference to the buffer frame. See @c BufferFrame
		 *         for more details.
		 */
		BufferFrame& fixPage(const PID& id, bool exclusive);

		/**
		 * Releases the given frame. Thus, the frame can be paged out,
		 * if necessary. Previous calls to @c BufferFrame::fixPage with
		 * the @c exclusive option will continue now.
		 */
		void unfixPage(BufferFrame& frame, bool isDirty);

	private:

		/**
		 * Creates a hash value for the specified page using an internal
		 * hashing algorithm.
		 *
		 * The hash is guaranteed not to exceed @c slotCount, if slotCount is
		 * a power of two. Otherwise, the hashed value is undefined.
		 *
		 * @param id The page identifier to obtain the hash value from.
		 * @return The hashed value of @c id.
		 */
		uint64_t hash(const PID& id) const;

		/**
		 * Returns a reference to the appropriate slot in the page table
		 * determined by the internal hash function.
		 *
		 * @param id The page identifier to obtain the hash value from.
		 * @return A reference to the slot.
		 */
		Slot& getSlot(const PID& id) const;

		/**
		 * Resolves a buffer frame within the given slot.
		 *
		 * @param slot A reference to the slot to search in.
		 * @param id   The id of the page.
		 *
		 * @return A pointer to the page frame, if found; otherwise @c nullptr.
		 */
		BufferFrame* acquirePage(Slot& slot, const PID& id);

		/**
		 * Registers an access to the specified page for the page replacement
		 * algorithm to prevent immediate page outs.
		 */
		void registerPageAccess(BufferFrame* frame);

		/**
		 * Creates a new buffer frame and allocates space for the page contents.
		 * If there is no more memory for new pages, an old page is written to 
		 * disk and removed from memory first
		 *
		 * @param slot A reference to the slot which will contain the frame.
		 * @param id   The id of the page.
		 *
		 * @return A pointer to the new page frame.
		 */
		BufferFrame* allocatePage(Slot& slot, const PID& id);

		/**
		 * Finds an unused page according to the page replacement algorithm.
		 * This method might fail, if there are no unused pages in memory.
		 * In this case an error message is printed and the function
		 * exits with code EXIT_CODE_NO_MORE_MEMORY.
		 * A BufferFrame is returned so that the memory can be reused and no new
		 * malloc calls needs to be done.
		 *
		 * @return BufferFrame* an unused buffer frame, removed from the queue
		 */
		BufferFrame* findUnusedPage();

		/**
		 * Finds the last unused page in the given queue and removes it from 
		 * memory.
		 *
		 * To find the last unused page, this method scans all BufferFrames 
		 * in this queue in reverse order. To prevent deadlocks, first the table 
		 * slot of the respective frame is locked (blocking). Then, an exclusive
		 * lock of the frame is attempted: 
		 *  - If this lock is successful, the page is removed from all lists and 
		 *    written to disc. 
		 *  - Otherwise, the frame is discarded and all locks are freed.
		 *
		 * This method might fail to remove a page, if there are no unused
		 * pages in the given queue.
		 *
		 * @return True, if a page was freed; otherwise false;
		 */
		BufferFrame* getLastUnusedPage(Queue& queue);

	};

}

#include "BufferManager.cpp"
