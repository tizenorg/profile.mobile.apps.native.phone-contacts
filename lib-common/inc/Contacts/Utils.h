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

#ifndef CONTACTS_UTILS_H
#define CONTACTS_UTILS_H

#include <contacts.h>
#include "Utils/String.h"

#define CONTACTS_LIST_FOREACH(list, record) \
	bool success = (contacts_list_get_current_record_p(list, &record) == CONTACTS_ERROR_NONE); \
	for ( ; success; \
			success = ((contacts_list_next(list) == CONTACTS_ERROR_NONE) \
			&& (contacts_list_get_current_record_p(list, &record) == CONTACTS_ERROR_NONE)) \
		)

namespace Contacts
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

	/**
	 * @brief Get display contact record for a person.
	 * @param[in]   personRecord    _contacts_person record
	 * @return Display contact's _contacts_contact record.
	 */
	contacts_record_h EXPORT_API getDisplayContact(contacts_record_h personRecord);

	/**
	 * @brief Get display contact ID by person ID.
	 * @param[in]   personId    _contacts_person ID
	 * @return Display contact's _contacts_contact ID.
	 */
	int EXPORT_API getDisplayContactId(int personId);

	/**
	 * @brief Get person ID by contact ID.
	 * @param[in]   contactId   _contacts_contact ID
	 * @return _contacts_person ID.
	 */
	int EXPORT_API getPersonId(int contactId);
}


#endif /* CONTACTS_UTILS_H */
