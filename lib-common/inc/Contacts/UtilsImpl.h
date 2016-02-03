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

#include "Contacts/Utils.h"

namespace Contacts
{
	inline ContactRecordChildIterator::ContactRecordChildIterator(contacts_record_h record,
			int property, size_t index)
		: IndexIterator(index), m_Record(record), m_Property(property)
	{
	}

	inline contacts_record_h ContactRecordChildIterator::operator*() const
	{
		contacts_record_h record = nullptr;
		contacts_record_get_child_record_at_p(m_Record, m_Property, getIndex(), &record);
		return record;
	}

	inline ContactRecordChildIterator begin(contacts_record_h record, int property)
	{
		return { record, property, 0 };
	}

	inline ContactRecordChildIterator end(contacts_record_h record, int property)
	{
		int count = 0;
		contacts_record_get_child_record_count(record, property, &count);
		return { record, property, size_t(count) };
	}

	inline Utils::Range<ContactRecordChildIterator> makeRange(contacts_record_h record, int property)
	{
		return { begin(record, property), end(record, property) };
	}
}

