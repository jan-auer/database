//
//  BufferManager.h
//  database
//
//  Created by Jan Michael Auer on 17/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#ifndef __database__BufferManager__
#define __database__BufferManager__

#include <cstdint>
#include <utility>
#include <vector>

#include "utils/Lock.h"
#include "BufferFrame.h"

namespace lsql {

	/**
	 * Manages buffer frames and controls concurrent access to these frames.
	 * Frames are loaded from and stored into a file on the disc. For more
	 * information about frames, see @c BufferFrame.
	 */
	class BufferManager {

		typedef std::pair<PageId, BufferFrame*> BufferEntry;
		typedef std::vector<BufferEntry> BufferSlot;

		Lock* slotLocks;
		BufferSlot* slots;
		uint64_t slotCount;

		File<void> file;
		uint64_t maxPages;
		
	public:
		
		/**
		 * Creates a new buffer manager instance which operates on the
		 * specified file.
		 *
		 * @param fileName Fully qualified path to the file containing
		 *                 the frames. The file will be opened with r+.
		 * @param size     The maximum number of frames in memory.
		 */
		BufferManager(const std::string& fileName, uint64_t size);
		
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
		 * fail to return a frame.
		 *
		 * @param id        An identifier for the page to load.
		 * @param exclusive Whether or not this frame is exclusive to the
		 *                  caller of this method.
		 *
		 * @return A reference to the buffer frame. See @c BufferFrame
		 *         for more details.
		 */
		BufferFrame& fixPage(const PageId& id, bool exclusive);
		
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
		uint64_t hash(const PageId& id) const;

		/**
		 * Resolves a buffer frame within the given slot.
		 *
		 * @param slot      A reference to the slot to search in.
		 * @param id        The id of the page.
		 * @param exclusive Whether or not the frame is exclusive to the caller 
		 *                  of this method.
		 *
		 * @return A pointer to the page frame, if found; otherwise @c nullptr.
		 */
		BufferFrame* acquirePage(BufferSlot& slot, const PageId& id, bool exclusive);

		/**
		 * Creates a new buffer frame and allocates space for the page contents. 
		 * 
		 * @param slot A reference to the slot which will contain the frame.
		 * @param id   The id of the page.
		 *
		 * @return A pointer to the new page frame.
		 */
		BufferFrame* allocatePage(BufferSlot& slot, const PageId& id);
		
	};
	
}

#include "BufferManager.cpp"

#endif /* defined(__database__BufferManager__) */
