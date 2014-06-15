//
//  TableScanOperator.cpp
//  database
//
//  Created by Jan Michael Auer on 14/06/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include "TableScanOperator.h"

namespace lsql {

	TableScanOperator::TableScanOperator(Relation& relation)
	:segment(relation), page(nullptr), isOpen(false) {
	}

	void TableScanOperator::open() {
		assert(!isOpen);
		
		pages = segment.begin();
		isOpen = true;
	}

	bool TableScanOperator::next() {
		assert(isOpen);

		while (pages != segment.end()) {
			if (page == nullptr) {
				page = &(*pages);
				tuples = page->begin();
			}

			if (tuples != page->end()) {
				for (Register* rp : output)
					delete rp;

				output = buildRow(*tuples);
				++tuples;
				return true;
			}

			++pages;
		}

		return false;
	}

	Row TableScanOperator::getOutput() const {
		assert(isOpen);
		return output;
	}

	void TableScanOperator::rewind() {
		assert(isOpen);

		close();
		open();
	}

	void TableScanOperator::close() {
		assert(isOpen);

		page = nullptr;
		pages = segment.end();
		isOpen = false;
	}

	Row TableScanOperator::buildRow(const Record& record) const {
		Row row(segment.attributes.size());
		const char* data = record.getData();

		for (auto& attribute : segment.attributes) {
			Register* r;

			switch (attribute.type) {
				case Type::Integer:
					r = new Register(*reinterpret_cast<const Integer*>(data));
					data += INTEGER_LEN;
					break;

				case Type::Char:
					r = new Register(Char(data, CHAR_LEN));
					data += CHAR_LEN;
					break;

				default:
					assert(false);
			}

			row.push_back(r);
		}

		return row;
	}

}