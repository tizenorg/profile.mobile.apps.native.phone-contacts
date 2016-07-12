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

#ifndef COMMON_DATABASE_RECORD_UTILS_H
#define COMMON_DATABASE_RECORD_UTILS_H

#include "Utils/String.h"
#include "Common/Database/ChildRecordIterator.h"

#include <contacts.h>

#define CONTACTS_LIST_FOREACH(list, record) \
	bool success = (contacts_list_get_current_record_p(list, &record) == CONTACTS_ERROR_NONE); \
	for ( ; success; \
			success = ((contacts_list_next(list) == CONTACTS_ERROR_NONE) \
			&& (contacts_list_get_current_record_p(list, &record) == CONTACTS_ERROR_NONE)) \
		)

namespace Common
{
	namespace Database
	{
		/**
		 * @brief Convenience wrapper.
		 * @see contacts_record_get_str_p()
		 */
		inline const char *getRecordStr(contacts_record_h record, unsigned propertyId)
		{
			char *str = nullptr;
			contacts_record_get_str_p(record, propertyId, &str);
			return str;
		}

		/**
		 * @brief Convenience wrapper.
		 * @see contacts_record_get_int()
		 */
		inline int getRecordInt(contacts_record_h record, unsigned propertyId)
		{
			int value = 0;
			contacts_record_get_int(record, propertyId, &value);
			return value;
		}

		/**
		 * @brief Convenience wrapper.
		 * @see contacts_record_get_bool()
		 */
		inline bool getRecordBool(contacts_record_h record, unsigned propertyId)
		{
			bool value = 0;
			contacts_record_get_bool(record, propertyId, &value);
			return value;
		}

		/**
		 * @brief Get first child record
		 * @remark Good way to get single child record
		 * @param[in]   record      Parent record
		 * @param[in]   property    @record property, that points to child DB view
		 * @return Child record on success, otherwise nullptr
		 */
		inline contacts_record_h getChildRecord(contacts_record_h record, unsigned property)
		{
			contacts_record_h childRecord = nullptr;
			contacts_record_get_child_record_at_p(record, property, 0, &childRecord);
			return childRecord;
		}

		/**
		 * @brief Get child record
		 * @param[in]   record      Parent record
		 * @param[in]   property    @record property, that points to child DB view
		 * @param[in]   predicate   Unary function, that accepts child record
		 *                          from parent record range and returns bool value
		 * @return Child record on success, otherwise nullptr
		 */
		template <typename Pred>
		inline contacts_record_h getChildRecord(contacts_record_h record, unsigned property,
				Pred predicate)
		{
			for (auto childRecord : makeRange(record, property)) {
				if (predicate(childRecord)) {
					return childRecord;
				}
			}

			return nullptr;
		}

		/**
		 * @brief Compare two records by the same string property.
		 * @param[in]   record1     First record
		 * @param[in]   records     Second record
		 * @param[in]   propertyId  Property to compare by
		 * @return Whether properties are equal.
		 */
		inline bool compareRecordsStr(contacts_record_h record1, contacts_record_h record2,
				unsigned propertyId)
		{
			return Utils::safeCmp(getRecordStr(record1, propertyId), getRecordStr(record2, propertyId));
		}

		/**
		 * @brief Compare two records by the same integer property.
		 * @see compareRecordsStr()
		 */
		inline bool compareRecordsInt(contacts_record_h record1, contacts_record_h record2,
				unsigned propertyId)
		{
			return getRecordInt(record1, propertyId) == getRecordInt(record2, propertyId);
		}
	}
}

#endif /* COMMON_DATABASE_RECORD_UTILS_H */
