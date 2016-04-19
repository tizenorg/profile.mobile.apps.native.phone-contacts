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

#ifndef COMMON_DATABASE_CHILD_RECORD_ITERATOR_H
#define COMMON_DATABASE_CHILD_RECORD_ITERATOR_H

#include "Utils/Iterator.h"
#include "Utils/Range.h"

namespace Common
{
	namespace Database
	{
		/**
		 * @brief Iterator for child record property.
		 */
		class ChildRecordIterator :
			public Utils::IndexIterator<ChildRecordIterator, contacts_record_h>
		{
		public:
			/**
			 * @brief Create child record iterator.
			 * @param[in]   record      Record containing child records
			 * @param[in]   property    Child record property ID
			 * @param[in]   index       Child record index
			 */
			ChildRecordIterator(contacts_record_h record, int property, size_t index)
				: IndexIterator(index), m_Record(record), m_Property(property)
			{
			}

			/**
			 * @brief Dereference iterator.
			 */
			contacts_record_h operator*() const
			{
				contacts_record_h record = nullptr;
				contacts_record_get_child_record_at_p(m_Record, m_Property, getIndex(), &record);
				return record;
			}

		private:
			contacts_record_h m_Record;
			int m_Property;
		};

		/**
		 * @see ChildRecordIterator::ChildRecordIterator()
		 */
		inline ChildRecordIterator begin(contacts_record_h record, int property)
		{
			return { record, property, 0 };
		}

		inline ChildRecordIterator end(contacts_record_h record, int property)
		{
			int count = 0;
			contacts_record_get_child_record_count(record, property, &count);
			return { record, property, size_t(count) };
		}

		inline Utils::Range<ChildRecordIterator> makeRange(contacts_record_h record, int property)
		{
			return { begin(record, property), end(record, property) };
		}
	}
}

#endif /* COMMON_DATABASE_CHILD_RECORD_ITERATOR_H */
