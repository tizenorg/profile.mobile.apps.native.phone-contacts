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

#ifndef COMMON_DATABASE_RECORD_ITERATOR_H
#define COMMON_DATABASE_RECORD_ITERATOR_H

#include <contacts.h>
#include "Utils/Range.h"

namespace Common
{
	namespace Database
	{
		/**
		 * @brief Iterator for record list.
		 */
		class RecordIterator :
			public std::iterator<std::bidirectional_iterator_tag, contacts_record_h>
		{
		public:
			/**
			 * @brief Create record list iterator.
			 * @param[in]   list        Contacts list
			 */
			RecordIterator(contacts_list_h list)
				: m_Index(0), m_ContactsList(list)
			{
			}

			/**
			 * @brief Create record list iterator.
			 * @param[in]   list        Contacts list
			 * @param[in]   index       Record list index
			 */
			RecordIterator(contacts_list_h list, size_t index)
				: m_Index(index), m_ContactsList(list)
			{
			}

			/**
			 * @brief Dereference iterator.
			 */
			contacts_record_h operator*() const
			{
				contacts_record_h record;
				contacts_list_get_current_record_p(m_ContactsList, &record);
				return record;
			}

			/**
			 * @brief Increment iterator.
			 */
			RecordIterator &operator++()
			{
				++m_Index;
				contacts_list_next(m_ContactsList);
				return *this;
			}

			/**
			 * @brief Decrement iterator.
			 */
			RecordIterator &operator--()
			{
				--m_Index;
				contacts_list_prev(m_ContactsList);
				return *this;
			}

			/**
			 * @brief Compare iterators.
			 */
			bool operator==(const RecordIterator &that) { return m_Index == that.m_Index; }
			bool operator!=(const RecordIterator &that) { return m_Index != that.m_Index; }

		private:
			size_t m_Index;
			contacts_list_h m_ContactsList;
		};

		inline RecordIterator begin(contacts_list_h list)
		{
			return { list, 0 };
		}

		inline RecordIterator end(contacts_list_h list)
		{
			int count = 0;
			contacts_list_get_count(list, &count);
			return { list, size_t(count) };
		}

		inline Utils::Range<RecordIterator> makeRange(contacts_list_h list)
		{
			return { begin(list), end(list) };
		}
	}
}

#endif /* COMMON_DATABASE_RECORD_ITERATOR_H */
