//
//  BufferFrame.h
//  database
//
//  Created by Jan Michael Auer on 17/04/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#ifndef __database__BufferFrame__
#define __database__BufferFrame__

#include <stdint.h>
#include <sys/types.h>

namespace lsql {

	/**
	 * A page id containing the unique segment identifier and
	 * the page identifier which is unique within the segment.
	 *
	 * The memory layout of PID enables it to be casted from
	 * a TID for better ease of use.
	 */
	struct PID {
		uint32_t page;
		uint16_t segment;
		
		PID(uint16_t segment, uint32_t page) : segment(segment), page(page) {}
	};
	
	/**
	 * Contains information about one page managed by a buffer manager.
	 */
	class BufferFrame {
		
		PID id;
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
		BufferFrame(const PID& id);
		
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
		
	};
	
}

#include "BufferFrame.cpp"

#endif /* defined(__database__BufferFrame__) */
