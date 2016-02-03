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
#include "Utils/Iterator.h"
#include "Utils/Range.h"

#define CONTACTS_LIST_FOREACH(list, record) \
	bool success = (contacts_list_get_current_record_p(list, &record) == CONTACTS_ERROR_NONE); \
	for ( ; success; \
			success = ((contacts_list_next(list) == CONTACTS_ERROR_NONE) \
			&& (contacts_list_get_current_record_p(list, &record) == CONTACTS_ERROR_NONE)) \
		)

namespace Contacts
{
	/**
	 * @brief Iterator for child record property.
	 */
	class ContactRecordChildIterator :
		public Utils::IndexIterator<ContactRecordChildIterator, contacts_record_h>
	{
	public:
		/**
		 * @brief Create child record iterator.
		 * @param[in]   record      Record containing child records
		 * @param[in]   property    Child record property ID
		 * @param[in]   index       Child record index
		 */
		ContactRecordChildIterator(contacts_record_h record, int property, size_t index);

		/**
		 * @brief Dereference iterator.
		 */
		contacts_record_h operator*() const;

	private:
		contacts_record_h m_Record;
		int m_Property;
	};

	/**
	 * @see ContactRecordChildIterator::ContactRecordChildIterator()
	 */
	ContactRecordChildIterator begin(contacts_record_h record, int property);
	ContactRecordChildIterator end(contacts_record_h record, int property);
	Utils::Range<ContactRecordChildIterator> makeRange(contacts_record_h record, int property);
}

#include "Contacts/UtilsImpl.h"

#endif /* CONTACTS_UTILS_H */
