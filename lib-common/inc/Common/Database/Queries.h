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

#ifndef COMMON_DATABASE_QUERIES_H
#define COMMON_DATABASE_QUERIES_H

#include <contacts.h>
#include "Utils/Range.h"

namespace Common
{
	namespace Database
	{
		/**
		 * @brief Get contact records for a person.
		 * @remark Return value MUST be destroyed with contacts_list_destroy by you
		 * @param[in]   personId    _contacts_person ID.
		 * @param[in]   projection  Query projection in _contacts_contact view
		 * @return _contacts_contact record list
		 */
		EXPORT_API contacts_list_h getPersonContacts(int personId, Utils::Range<unsigned *> projection = { });

		/**
		 * @brief Get display contact ID by person ID.
		 * @param[in]   personId    _contacts_person ID
		 * @return Display contact's _contacts_contact ID.
		 */
		EXPORT_API int getDisplayContactId(int personId);

		/**
		 * @brief Get person ID by contact ID.
		 * @param[in]   contactId   _contacts_contact ID
		 * @return _contacts_person ID.
		 */
		EXPORT_API int getPersonId(int contactId);

		/**
		 * @brief Get record list
		 * @remark Return value MUST be destroyed with contacts_list_destroy by you
		 * @param[in]   uri             Record uri
		 * @param[in]   propertyId      Filter property ID
		 * @param[in]   value           Value that should be equal to @a propertyId field value
		 * @param[in]   parentFilter    Parent filter, optional
		 * @return Contacts list on success, otherwise nullptr
		 */
		EXPORT_API contacts_list_h getRecordList(const char *uri, unsigned propertyId, int value, contacts_filter_h parentFilter = nullptr);

		/**
		 * @brief Get record list
		 * @remark Return value MUST be destroyed with contacts_list_destroy by you
		 * @param[in]   uri             Record uri
		 * @param[in]   propertyId      Filter property ID
		 * @param[in]   values          List of values, which should be equal with @a propertyId field value
		 * @param[in]   parentFilter    Parent filter, optional
		 * @return Contacts list on success, otherwise nullptr
		 */
		EXPORT_API contacts_list_h getRecordList(const char *uri, unsigned propertyId, Utils::Range<int *> values, contacts_filter_h parentFilter = nullptr);
	}
}

#endif /* COMMON_DATABASE_QUERIES_H */
