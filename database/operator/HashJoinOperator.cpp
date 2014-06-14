//
//  HashJoinOperator.cpp
//  database
//
//  Created by Jan Michael Auer on 14/06/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include <cassert>
#include "HashJoinOperator.h"

namespace lsql {

	HashJoinOperator::HashJoinOperator(IOperator& left, IOperator& right, uint16_t leftIndex, uint16_t rightIndex)
	: build(left), probe(right), buildIndex(leftIndex), probeIndex(rightIndex), isOpen(false) {
	}

	void HashJoinOperator::open() {
		assert(!isOpen);

		build.open();
		while (build.next()) {
			Row row = build.getOutput();
			map[*row[buildIndex]].push_back(std::move(copyRow(row)));
		}
		build.close();

		probe.open();
		isOpen = true;
	}

	bool HashJoinOperator::next() {
		assert(isOpen);

		if (lastBuilds.size() > 0) {
			output = std::move(join(lastProbe, pop(lastBuilds)));
			return true;
		}

		while (probe.next()) {
			lastProbe = probe.getOutput();
			Register& probeJoinValue = *lastProbe[probeIndex];

			if (map.count(probeJoinValue) > 0) {
				lastBuilds = map[probeJoinValue];
				output = std::move(join(lastProbe, pop(lastBuilds)));
			}
		}

		return false;
	}

	Row HashJoinOperator::getOutput() const {
		assert(isOpen);
		return output;
	}

	void HashJoinOperator::rewind() {
		assert(isOpen);
		probe.rewind();
	}

	void HashJoinOperator::close() {
		assert(isOpen);

		probe.close();

		for (auto entry : map)
			for (auto row : entry.second)
				for (auto reg : row)
					delete reg;

		map.clear();
		isOpen = false;
	}

	Row HashJoinOperator::copyRow(const Row& row) const {
		Row copy;
		copy.reserve(row.size());

		for (Register* r : row)
			copy.push_back(new Register(*r));

		return copy;
	}

	Row HashJoinOperator::join(const Row& left, const Row& right) const {
		Row join(left);
		join.insert(join.end(), right.begin(), right.end());
		return join;
	}

}