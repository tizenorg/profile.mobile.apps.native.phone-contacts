/*
 * Copyright (c) 2015-2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef UTILS_ITERATOR_H
#define UTILS_ITERATOR_H

#include <iterator>
#include <stddef.h>

namespace Utils
{
	/**
	 * @brief Base class for index-based iterators.
	 * @details CRTP (Curiously Recurring Template Pattern) is used to generate
	 *          operators that return or use derived type.
	 */
	template <typename DerivedIterator,
		typename ValueType,
		typename Pointer = ValueType *,
		typename Reference = ValueType &,
		typename Distance = ptrdiff_t>
	class IndexIterator :
		public std::iterator<std::bidirectional_iterator_tag, ValueType, Distance, Pointer, Reference>
	{
	public:
		/**
		 * @brief Create iterator from index.
		 * @param[in]   index   Current position of iterator
		 */
		IndexIterator(size_t index = 0)
			: m_Index(index) { }

		/**
		 * @brief Dereference iterator.
		 */
		Pointer operator->() const { return &*getDerived(); }

		/**
		 * @brief Increment iterator.
		 */
		DerivedIterator &operator++() { ++m_Index; return getDerived(); }
		DerivedIterator operator++(int) { auto it(getDerived()); ++*this; return it; }

		/**
		 * @brief Decrement iterator.
		 */
		DerivedIterator &operator--() { --m_Index; return getDerived(); }
		DerivedIterator operator--(int) { auto it(getDerived()); --*this; return it; }

		/**
		 * @brief Compare iterators.
		 */
		bool operator==(const IndexIterator &that) { return m_Index == that.m_Index; }
		bool operator!=(const IndexIterator &that) { return m_Index != that.m_Index; }

	protected:
		/**
		 * @return Current index.
		 */
		size_t getIndex() const { return m_Index; }

	private:
		DerivedIterator &getDerived() { return static_cast<DerivedIterator &>(*this); }
		const DerivedIterator &getDerived() const { return static_cast<const DerivedIterator &>(*this); }

		size_t m_Index;
	};
}

#endif /* UTILS_ITERATOR_H */
