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

#include "Contacts/List/Model/Person.h"
#include "Contacts/Utils.h"

#include <cstring>

using namespace Contacts::List::Model;
using namespace Utils;

namespace
{
	unsigned getSortProperty(contacts_name_sorting_order_e order)
	{
		return (order == CONTACTS_NAME_SORTING_ORDER_FIRSTLAST
				? _contacts_name.first : _contacts_name.last);
	}

	Person::ContactIds getContactIds(int personId)
	{
		unsigned projection = _contacts_contact.id;
		Person::ContactIds ids;

		contacts_filter_h filter = nullptr;
		contacts_filter_create(_contacts_contact._uri, &filter);
		contacts_filter_add_int(filter, _contacts_contact.person_id, CONTACTS_MATCH_EQUAL, personId);

		contacts_query_h query = nullptr;
		contacts_query_create(_contacts_contact._uri, &query);
		contacts_query_set_filter(query, filter);
		contacts_query_set_projection(query, &projection, 1);

		contacts_list_h list = nullptr;
		contacts_db_get_records_with_query(query, 0, 0, &list);

		contacts_record_h tempRecord = nullptr;
		CONTACTS_LIST_FOREACH(list, tempRecord) {
			int id = 0;
			contacts_record_get_int(tempRecord, projection, &id);
			ids.push_back(id);
		}

		contacts_list_destroy(list, true);
		contacts_query_destroy(query);
		contacts_filter_destroy(filter);

		return ids;
	}
}

Person::Person(contacts_record_h record)
	: ContactRecordData(TypePerson), m_PersonRecord(nullptr)
{
	initialize(record, getDisplayContact(record));
}

Person::~Person()
{
	contacts_record_destroy(m_PersonRecord, true);
}

int Person::getId() const
{
	int id = 0;
	contacts_record_get_int(m_PersonRecord, _contacts_person.id, &id);
	return id;
}

int Person::getDisplayContactId() const
{
	int id = 0;
	contacts_record_get_int(m_PersonRecord, _contacts_person.display_contact_id, &id);
	return id;
}

const Person::ContactIds &Person::getContactIds() const
{
	return m_ContactIds;
}

const UniString &Person::getIndexLetter() const
{
	return m_IndexLetter;
}

const contacts_record_h Person::getRecord() const
{
	return m_PersonRecord;
}

bool Person::operator<(const Person &that) const
{
	return getSortValue() < that.getSortValue();
}

void Person::initialize(contacts_record_h personRecord, contacts_record_h contactRecord)
{
	m_PersonRecord = personRecord;
	updateContactRecord(contactRecord);
	m_ContactIds = ::getContactIds(getId());

	char *indexLetter = nullptr;
	contacts_record_get_str_p(m_PersonRecord, _contacts_person.display_name_index, &indexLetter);
	m_IndexLetter = indexLetter;
}

int Person::updatePerson(contacts_record_h personRecord)
{
	contacts_record_h contactRecord = getDisplayContact(personRecord);
	auto changes = getChanges(contactRecord);

	contacts_record_destroy(m_PersonRecord, true);
	m_SortValue.clear();
	initialize(personRecord, contactRecord);

	return changes;
}

const UniString &Person::getSortValue() const
{
	if (m_SortValue.getI18nStr().empty()) {
		m_SortValue = getDbSortValue();
	}

	return m_SortValue;
}

const char *Person::getDbSortValue() const
{
	contacts_name_sorting_order_e order;
	contacts_setting_get_name_sorting_order(&order);
	unsigned sortField = getSortProperty(order);

	contacts_record_h nameRecord = nullptr;
	contacts_record_get_child_record_at_p(getContactRecord(), _contacts_contact.name, 0, &nameRecord);

	char *sortValue = nullptr;
	contacts_record_get_str_p(nameRecord, sortField, &sortValue);

	return sortValue;
}

int Person::getChanges(contacts_record_h record)
{
	auto isChanged = [](const char *str1, const char *str2) {
		return (str1 && str2)
				? (strcmp(str1, str2) != 0)
				: (str1 != str2);
	};

	int changes = ChangedNone;

	for (int i = ContactData::FieldFirst; i < ContactData::FieldMax; ++i) {
		auto fieldId = static_cast<ContactData::Field>(i);

		auto oldValue = getValue(getContactRecord(), fieldId);
		auto newValue = getValue(record, fieldId);
		auto changedInfo = 1 << fieldId;

		if (isChanged(oldValue, newValue)) {
			changes |= changedInfo;
		}
	}

	return changes;
}
