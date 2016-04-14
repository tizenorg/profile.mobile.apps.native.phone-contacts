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
#include "Contacts/RecordListIterator.h"
#include "Contacts/Utils.h"

#include <cstring>

using namespace Contacts;
using namespace Contacts::Model;
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
	: ContactRecordData(TypePerson, getDisplayContact(record)), m_PersonRecord(nullptr)
{
	initialize(record);
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

const Person::Numbers &Person::getNumbers()
{
	unsigned projection[] = {
		_contacts_contact.number
	};

	if (getContactNumbers().empty()) {
		contacts_list_h list = getContacts(m_PersonRecord, projection);
		auto contacts = makeRange(list);
		for (auto &&contact : contacts) {
			fillContactNumbers(contact);
		}

		contacts_list_destroy(list, false);
	}

	return getContactNumbers();
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

void Person::setChangedCallback(Contacts::Model::DbChangeObserver::Callback callback)
{
	for (auto &&id : m_ContactIds) {
		auto handle = DbChangeObserver::getInstance()->addCallback(id, callback);
		addChangedHandle(handle);
	}
}

void Person::unsetChangedCallback()
{
	for (size_t i = 0; i < m_ContactIds.size(); ++i) {
		DbChangeObserver::getInstance()->removeCallback(m_ContactIds[i], getChangedHandle(i));
	}
	clearChangedHandles();
}

void Person::onUpdate(contacts_record_h personRecord)
{
	contacts_record_h contactRecord = getDisplayContact(personRecord);
	auto changes = getChanges(getContactRecord(), contactRecord);

	contacts_record_destroy(m_PersonRecord, true);
	m_SortValue.clear();

	updateRecord(contactRecord);
	initialize(personRecord);

	onUpdated(changes);
}

void Person::initialize(contacts_record_h personRecord)
{
	m_PersonRecord = personRecord;
	m_ContactIds = ::getContactIds(getId());

	char *indexLetter = nullptr;
	contacts_record_get_str_p(m_PersonRecord, _contacts_person.display_name_index, &indexLetter);
	m_IndexLetter = indexLetter;
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

	if (!(sortValue && *sortValue)) {
		contacts_record_get_str_p(getContactRecord(), _contacts_contact.display_name, &sortValue);
	}

	return sortValue;
}
