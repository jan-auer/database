//
//  TableScanOperator.h
//  database
//
//  Created by Jan Michael Auer on 14/06/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#pragma once

#include "schema/Relation.h"
#include "segment/SlottedPage.h"
#include "segment/SPSegment.h"
#include "IOperator.h"

namespace lsql {

	/**
	 *
	 */
	class TableScanOperator : public IOperator {

		Relation& segment;
		SlottedPage* page;

		SPSegment::Iterator pages;
		SlottedPage::Iterator tuples;

		bool isOpen;
		Row output;

	public:

		/**
		 *
		 */
		TableScanOperator(Relation& relation);

		// IOperator interface implementation.

		void open();
		bool next();
		Row getOutput() const;
		void rewind();
		void close();

	private:

		/**
		 * 
		 */
		Row buildRow(const Record& record) const;

	};

}