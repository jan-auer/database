//
//  Serialize-impl.h
//  database
//
//  Created by Jan Michael Auer on 15/05/14.
//  Copyright (c) 2014 LightningSQL. All rights reserved.
//

#include <string>
#include <tuple>
#include <numeric>
#include <cassert>

namespace lsql {

	template <class T>
	size_t get_size(const T& obj);

	namespace detail {

#pragma mark Size Helpers

		template<std::size_t> struct int_{};

		template <class T>
		struct get_size_helper<std::vector<T>> {

			static size_t value(const std::vector<T>& obj) {
				auto getSize = [](const size_t& acc, const T& cur) {
					return acc+get_size(cur);
				};

				return std::accumulate(obj.begin(), obj.end(), sizeof(size_t), getSize);
			}

		};

		template <>
		struct get_size_helper<std::string> {

			static size_t value(const std::string& obj) {
				return sizeof(size_t) + obj.length()*sizeof(uint8_t);
			}

		};

		template <class tuple_type>
		inline size_t get_tuple_size(const tuple_type& obj, int_<0>) {
			constexpr size_t idx = std::tuple_size<tuple_type>::value-1;
			return get_size(std::get<idx>(obj));
		}

		template <class tuple_type, size_t pos>
		inline size_t get_tuple_size(const tuple_type& obj, int_<pos>) {
			constexpr size_t idx = std::tuple_size<tuple_type>::value-pos-1;
			size_t acc = get_size(std::get<idx>(obj));

			// recur
			return acc+get_tuple_size(obj, int_<pos-1>());
		}

		template <class ...T>
		struct get_size_helper<std::tuple<T...>> {

			static size_t value(const std::tuple<T...>& obj) {
				return get_tuple_size(obj, int_<sizeof...(T)-1>());
			}

		};

		template <class T>
		struct get_size_helper {
			static size_t value(const T& obj) { return sizeof(T); }
		};

#pragma mark Serialize Helpers

		template <class T>
		void serializer(const T& obj, StreamType::iterator&);

		template <class tuple_type>
		inline void serialize_tuple(const tuple_type& obj, StreamType::iterator& res, int_<0>) {
			constexpr size_t idx = std::tuple_size<tuple_type>::value-1;
			serializer(std::get<idx>(obj), res);
		}

		template <class tuple_type, size_t pos>
		inline void serialize_tuple(const tuple_type& obj, StreamType::iterator& res, int_<pos>) {
			constexpr size_t idx = std::tuple_size<tuple_type>::value-pos-1;
			serializer(std::get<idx>(obj), res);

			// recur
			serialize_tuple(obj, res, int_<pos-1>());
		}

		template <class... T>
		struct serialize_helper<std::tuple<T...>> {

			static void apply(const std::tuple<T...>& obj, StreamType::iterator& res) {
				detail::serialize_tuple(obj, res, detail::int_<sizeof...(T)-1>());
			}

		};

		template <>
		struct serialize_helper<std::string> {

			static void apply(const std::string& obj, StreamType::iterator& res) {
				// store the number of elements of this vector at the beginning
				serializer(obj.length(), res);
				for(const auto& cur : obj) {
					serializer(cur, res);
				}
			}

		};

		template <class T>
		struct serialize_helper<std::vector<T>> {

			static void apply(const std::vector<T>& obj, StreamType::iterator& res) {
				// store the number of elements of this vector at the beginning
				serializer(obj.size(), res);
				for(const auto& cur : obj) {
					serializer(cur, res);
				}
			}

		};

		template <class T>
		struct serialize_helper {

			static void apply(const T& obj, StreamType::iterator& res) {
				const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&obj);
				std::copy(ptr,ptr+sizeof(T),res);
				res+=sizeof(T);
			}

		};

		template <class T>
		inline void serializer(const T& obj, StreamType::iterator& res) {
			serialize_helper<T>::apply(obj,res);
		}

#pragma mark Deserialize Helpers

		template <class T, class C>
		struct deserialize_helper {

			static T apply(StreamType::const_iterator& begin,
										 StreamType::const_iterator end,
										 C* context) {
				assert(begin+sizeof(T)<=end);
				T val;
				uint8_t* ptr = reinterpret_cast<uint8_t*>(&val);
				std::copy(begin, begin+sizeof(T), ptr);
				begin+=sizeof(T);
				return val;
			}

		};

		template <class T, class C>
		struct deserialize_helper<std::vector<T>, C> {

			static std::vector<T> apply(StreamType::const_iterator& begin,
																	StreamType::const_iterator end,
																	C* context) {
				// retrieve the number of elements
				size_t size = deserialize_helper<size_t, C>::apply(begin,end,context);

				std::vector<T> vect(size);
				for(size_t i=0; i<size; ++i) {
					vect[i] = std::move(deserialize_helper<T,C>::apply(begin,end,context));
				}

				return vect;
			}
		};

		template <>
		struct deserialize_helper<std::string, void> {

			static std::string apply(StreamType::const_iterator& begin,
															 StreamType::const_iterator end,
															 void* context) {
				// retrieve the number of elements
				size_t size = deserialize_helper<size_t, void>::apply(begin,end,nullptr);

				if (size == 0u) return std::string();

				std::string str(size,'\0');
				for(size_t i=0; i<size; ++i) {
					str.at(i) = deserialize_helper<uint8_t, void>::apply(begin,end,context);
				}
				return str;
			}
		};

		template <class tuple_type, class C>
		inline void deserialize_tuple(tuple_type& obj, StreamType::const_iterator& begin,
																	StreamType::const_iterator end, int_<0>, C* context) {
			constexpr size_t idx = std::tuple_size<tuple_type>::value-1;
			typedef typename std::tuple_element<idx,tuple_type>::type T;

			std::get<idx>(obj) = std::move(deserialize_helper<T>::apply(begin, end, context));
		}

		template <class tuple_type, size_t pos, class C>
		inline void deserialize_tuple(tuple_type& obj, StreamType::const_iterator& begin,
																	StreamType::const_iterator end, int_<pos>,
																	C* context) {
			constexpr size_t idx = std::tuple_size<tuple_type>::value-pos-1;
			typedef typename std::tuple_element<idx,tuple_type>::type T;

			std::get<idx>(obj) = std::move(deserialize_helper<T>::apply(begin, end, context));

			// recur
			deserialize_tuple(obj, begin, end, int_<pos-1>(), context);
		}

		template <class... T, class C>
		struct deserialize_helper<std::tuple<T...>, C> {

			static std::tuple<T...> apply(StreamType::const_iterator& begin,
																		StreamType::const_iterator end,
																		C* context) {
				//return std::make_tuple(deserialize(begin,begin+sizeof(T),T())...);
				std::tuple<T...> ret;
				deserialize_tuple(ret, begin, end, int_<sizeof...(T)-1>(), context);
				return ret;
			}

		};

	}

#pragma mark Interface Implementation

	template <class T>
	inline size_t get_size(const T& obj) {
		return detail::get_size_helper<T>::value(obj);
	}

	template <class T>
	inline void serialize(const T& obj, StreamType& res) {
		size_t offset = res.size();
		size_t size = get_size(obj);
		res.resize(res.size() + size);

		StreamType::iterator it = res.begin()+offset;
		detail::serializer(obj,it);
		assert(res.begin() + offset + size == it);
	}

	template <class T, class C = void>
	inline T deserialize(StreamType::const_iterator& begin,
											 const StreamType::const_iterator& end,
											 C* context) {
		return detail::deserialize_helper<T, C>::apply(begin, end, context);
	}

	template <class T, class C>
	inline T deserialize(const StreamType& res, C* context) {
		StreamType::const_iterator it = res.begin();
		return deserialize<T>(it, res.end());
	}

}
