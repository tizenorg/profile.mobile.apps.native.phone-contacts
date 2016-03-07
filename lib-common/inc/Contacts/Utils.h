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

#define CONTACTS_LIST_FOREACH(list, record) \
	bool success = (contacts_list_get_current_record_p(list, &record) == CONTACTS_ERROR_NONE); \
	for ( ; success; \
			success = ((contacts_list_next(list) == CONTACTS_ERROR_NONE) \
			&& (contacts_list_get_current_record_p(list, &record) == CONTACTS_ERROR_NONE)) \
		)

namespace Contacts
{
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
