/**
 * Copyright (c) 2013, Simone Pellegrini All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  - Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 *  - Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <cstdint>
#include <vector>

namespace lsql {

	typedef std::vector<uint8_t> StreamType;

	/**
	 * Serializes the given data object into the provided stream.
	 *
	 * In order to be serializable, @c detail::size_helper and 
	 * @c detail::serialize_helper have to be overloaded.
	 *
	 * @param obj A data object.
	 * @param res The resource stream which will receive the serialized data.
	 */
	template <class T>
	inline void serialize(const T& obj, StreamType& res);

	/**
	 * Serializes a data object from the provided stream.
	 *
	 * In order to provide additional information to the deserialization function,
	 * an optional (mutable) context may be passed to the deserializer. By default,
	 * the context is of type @c void, but it may be adapted to any needs.
	 *
	 * In order to be deserializable, @c detail::size_helper<T> and
	 * @c detail::deserialize_helper<T> have to be implemented.
	 *
	 * @param res     The resource stream which contains serialized data.
	 * @param context An optional context to provide during deserialization.
	 */
	template <class T, class Context = void>
	inline T deserialize(const StreamType& res, Context* context = nullptr);

	/**
	 * Contains interfaces and implementation for the serialization.
	 */
	namespace detail {

		/**
		 * A struct needed during serialization and deserialization to calculate the
		 * size of the serialized object.
		 *
		 * The @c get_serialize_helper struct contains one static method to calculate
		 * the serialized size of an object.
		 * \code 
		 *    static size_t value(const T& obj);
		 * \endcode
		 *
		 * The default implementation returns @c sizeof(T), which will match the
		 * default implementation of @c serialize_helper and @c deserialize_helper. 
		 * In case there is special serialization desired, this method should be
		 * overloaded by implementing @c get_size_helper for the concrete type.
		 */
		template <class T>
		struct get_size_helper;

		/**
		 * A struct needed to serialize an object of type @c T. 
		 *
		 * The @c serialize_helper struct contains one static method which writes 
		 * an object to a stream:
		 * \code
		 *    static void apply(const T& obj, StreamType::iterator& res);
		 * \endcode
		 *
		 * The default implementation writes the memory representation of the object
		 * directly to the stream. This works for enums, simple types and classes
		 * without pointers or references. In case there is special serialization 
		 * desired, this method should be overloaded by implementing @c serialize_helper
		 * for the concrete type.
		 */
		template <class T>
		class serialize_helper;

		/**
		 * A struct needed to deserialize an object of type @c T.
		 *
		 * The @c deserialize_helper struct contains one static method which reads 
		 * an object from a stream:
		 * \code
		 *    static T apply(StreamType::const_iterator& begin, 
		 *                   StreamType::const_iterator end,
		 *                   Context* context = nullptr);
		 * \endcode
		 *
		 * The default implementation directly maps the stream data into the memory
		 * for @c sizeof(T) bytes. This works for enums, simple types and classes 
		 * without pointers or references. In case there is special deserialization
		 * desired, this method should be overloaded by implementing @c deserialize_helper
		 * for the concrete type.
		 *
		 * If additional information is needed during deserialization, the @c Context
		 * type may be changed to any kind of (mutable) struct or class which 
		 * reflects in the @c apply method interface.
		 */
		template <class T, class Context = void>
		struct deserialize_helper;

	}

}

#include "Serialize-impl.h"
