/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef UTILS_RANGE_H
#define UTILS_RANGE_H

#include <iterator>

namespace Utils
{
	/**
	 * @brief Convenience wrapper around std::advance.
	 * @param[in]   iterator    Input iterator
	 * @param[in]   distance    Distance to advance iterator
	 * @return New iterator at @a distance from @a iterator.
	 */
	template <typename Iter, typename Distance>
	inline Iter advance(Iter iterator, Distance distance)
	{
		std::advance(iterator, distance);
		return iterator;
	}

	/**
	 * @brief Convenience wrapper for iterator pair.
	 */
	template <typename Iter>
	class Range
	{
	public:
		typedef typename std::iterator_traits<Iter>::reference reference;

		Range() = default;

		/**
		 * @brief Create iterator range from static array.
		 * @param[in]   array   Static array
		 */
		template <typename T, size_t N>
		Range(T(&array)[N])
			: m_Begin(array), m_End(array + N) { }

		/**
		 * @brief Create iterator range from begin iterator and element count
		 * @param[in]   begin   Begin iterator
		 * @param[in]   count   Count of elements until range's end
		 */
		Range(Iter begin, size_t count)
			: m_Begin(std::move(begin)), m_End(advance(m_Begin, count)) { }

		/**
		 * @brief Create iterator range from pair of iterators
		 * @param[in]   begin   Begin iterator
		 * @param[in]   end     End iterator
		 */
		Range(Iter begin, Iter end)
			: m_Begin(std::move(begin)), m_End(std::move(end)) { }

		/**
		 * @return Whether range is empty (i. e. begin() == end()).
		 */
		bool empty() const { return !(m_Begin != m_End); }

		/**
		 * @return Range element count.
		 */
		size_t count() const { return std::distance(m_Begin, m_End); }

		/**
		 * @return Range begin iterator.
		 */
		Iter &begin() { return m_Begin; }

		/**
		 * @return Range end iterator.
		 */
		Iter &end() { return m_End; }

		/**
		 * @return Range begin iterator.
		 */
		const Iter &begin() const { return m_Begin; }

		/**
		 * @return Range end iterator.
		 */
		const Iter &end() const { return m_End; }

		/**
		 * @see empty()
		 */
		explicit operator bool() const { return !empty(); }

		/**
		 * @brief Get range element by index.
		 * @param[in]   index   Element index
		 * @return Range element reference.
		 */
		reference operator[](size_t index) const { return *advance(m_Begin, index); }

	private:
		Iter m_Begin;
		Iter m_End;
	};

	/**
	 * @brief Create iterator range from static array.
	 */
	template <typename T, size_t N>
	inline Range<T*> makeRange(T(&array)[N])
	{
		return { array };
	}

	/**
	 * @brief Create iterator range from two iterators.
	 */
	template <typename Iter1, typename Iter2,
		typename Iter = typename std::common_type<Iter1, Iter2>::type>
	inline Range<Iter> makeRange(Iter1 &&begin, Iter2 &&end)
	{
		return { std::forward<Iter1>(begin), std::forward<Iter2>(end) };
	}

	/**
	 * @brief Get static array element count.
	 * @param[in]   array   Static array
	 * @return Array element count.
	 */
	template <typename T, size_t N>
	inline size_t count(T(&array)[N])
	{
		return N;
	}

	/**
	 * @brief Get element by index from static array or a default value if out of bounds.
	 * @param[in]   array   Static array
	 * @param[in]   index   Element index
	 * @param[in]   value   Default value
	 * @return Array element or @a value. Array element is always copied.
	 */
	template <typename T, size_t N>
	inline T at(T(&array)[N], size_t index, T value = T())
	{
		return index < N ? array[index] : value;
	}
}

#endif /* UTILS_RANGE_H */
