//
//  HashJoinOperator.h
//  database
//
//  Created by Jan Michael Auer on 14/06/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#pragma once

#include <vector>
#include <unordered_map>

#include "IOperator.h"

namespace lsql {

	/**
	 * The Hash Join operator is initialized with two input operators, and
	 * two register IDs. One ID is from the left side and one is from the
	 * right side.
	 */
	class HashJoinOperator : public IOperator {

		bool isOpen;

		IOperator& build;
		IOperator& probe;

		uint16_t buildIndex;
		uint16_t probeIndex;

		// uses std::hash<Register> as hash function
		std::unordered_map<Register, std::vector<Row>> map;

		std::vector<Row> lastBuilds;
		Row lastProbe;
		Row output;

	public:

		/**
		 * Creates a new operator:
		 * Hash Join: Compute inner join by storing left input in main memory,
		 * then find matches for each tuple from the right side. The predicate
		 * is of the form left.a = right.b.
		 *
		 * @param left
		 * @param right
		 * @param leftIndex
		 * @param rightIndex
		 */
		HashJoinOperator(IOperator& left, IOperator& right, uint16_t leftIndex, uint16_t rightIndex);

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
		Row copyRow(const Row& row) const;

		/**
		 *
		 */
		Row join(const Row& left, const Row& right) const;

		/**
		 *
		 */
		template<typename T>
		T&& pop(std::vector<T> v) const {
			T t = std::move(v.back());
			v.pop_back();
			return std::move(t);
		}

	};

}