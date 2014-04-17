//
//  BufferManager.h
//  database
//
//  Created by Jan Michael Auer on 17/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#ifndef __database__BufferManager__
#define __database__BufferManager__

#include "BufferFrame.h"

namespace lsql {

	/**
	 * Manages buffer frames and controls concurrent access to these frames.
	 * Frames are loaded from and stored into a file on the disc. For more
	 * information about frames, see @c BufferFrame.
	 */
	class BufferManager {
		
	public:
		
		/**
		 * Creates a new buffer manager instance which operates on the
		 * specified file.
		 *
		 * @param fileName Fully qualified path to the file containing
		 *                 the frames. The file will be opened with r+.
		 * @param size     The maximum number of frames in memory.
		 */
		BufferManager(const std::string& fileName, size_t size);
		
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
		BufferFrame& fixPage(PID id, bool exclusive);
		
		/**
		 * Releases the given frame. Thus, the frame can be paged out,
		 * if necessary. Previous calls to @c BufferFrame::fixPage with
		 * the @c exclusive option will continue now.
		 */
		void unfixPage(BufferFrame& frame, bool isDirty);
		
	};
	
}

#include "BufferManager.cpp"

#endif /* defined(__database__BufferManager__) */
