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
#include "Contacts/Model/ContactNumberData.h"
#include "Common/Database/Queries.h"
#include "Common/Database/RecordUtils.h"
#include "Common/Database/RecordIterator.h"
#include "Common/Database/ChildRecordIterator.h"
#include "Utils/Logger.h"

using namespace Common::Database;
using namespace Contacts;
using namespace Contacts::Model;
using namespace Contacts::List::Model;
using namespace Utils;

namespace
{
	unsigned getSortProperty()
	{
		contacts_name_sorting_order_e order;
		contacts_setting_get_name_sorting_order(&order);
		return (order == CONTACTS_NAME_SORTING_ORDER_FIRSTLAST
				? _contacts_name.first : _contacts_name.last);
	}

	contacts_record_h getNameRecord(int contactId)
	{
		unsigned projection[] = {
			_contacts_name.first,
			_contacts_name.last
		};

		contacts_filter_h filter = nullptr;
		contacts_filter_create(_contacts_name._uri, &filter);
		contacts_filter_add_int(filter, _contacts_name.contact_id, CONTACTS_MATCH_EQUAL, contactId);

		contacts_query_h query = nullptr;
		contacts_query_create(_contacts_name._uri, &query);
		contacts_query_set_filter(query, filter);
		contacts_query_set_projection(query, projection, Utils::count(projection));

		contacts_list_h list = nullptr;
		int err = contacts_db_get_records_with_query(query, 0, 1, &list);
		WARN_IF_ERR(err, "contacts_db_get_records_with_query() failed.");

		contacts_record_h record = nullptr;
		contacts_list_get_current_record_p(list, &record);

		contacts_list_destroy(list, false);
		contacts_query_destroy(query);
		contacts_filter_destroy(filter);

		return record;
	}

	contacts_record_h getNumberRecord(int personId)
	{
		int id = 0;
		contacts_record_h record = nullptr;
		contacts_person_get_default_property(CONTACTS_PERSON_PROPERTY_NUMBER, personId, &id);
		contacts_db_get_record(_contacts_number._uri, id, &record);

		return record;
	}
}

Person::Person(contacts_record_h record)
	: ContactData(TypePerson),
	  m_Record(record), m_NameRecord(nullptr), m_NumberRecord(nullptr),
	  m_SortProperty(getSortProperty())
{
	m_IndexLetter = getRecordStr(m_Record, _contacts_person.display_name_index);
}

Person::~Person()
{
	for (auto &&number : m_Numbers) {
		delete number;
	}
	contacts_record_destroy(m_NameRecord, true);
	contacts_record_destroy(m_NumberRecord, true);
	contacts_record_destroy(m_Record, true);
}

int Person::getId() const
{
	return getRecordInt(m_Record, _contacts_person.id);
}

const char *Person::getName() const
{
	return getRecordStr(m_Record, _contacts_person.display_name);
}

const char *Person::getNumber() const
{
	if (!m_NumberRecord) {
		m_NumberRecord = getNumberRecord(getId());
	}
	return getRecordStr(m_NumberRecord, _contacts_number.number);
}

const char *Person::getImagePath() const
{
	return getRecordStr(m_Record, _contacts_person.image_thumbnail_path);
}

contacts_record_h Person::getRecord() const
{
	return m_Record;
}

int Person::getContactId() const
{
	return getRecordInt(m_Record, _contacts_person.display_contact_id);
}

const Person::Numbers &Person::getNumbers()
{
	unsigned projection[] = {
		_contacts_contact.number
	};

	if (m_Numbers.empty()) {
		contacts_list_h list = getPersonContacts(getId(), projection);
		for (auto &&contact : makeRange(list)) {
			for (auto &&number : makeRange(contact, _contacts_contact.number)) {
				m_Numbers.push_back(new ContactNumberData(*this, number));
			}
			contacts_record_destroy(contact, false);
		}
		contacts_list_destroy(list, false);
	}

	return m_Numbers;
}

const UniString &Person::getIndexLetter() const
{
	return m_IndexLetter;
}

bool Person::operator<(const Person &that) const
{
	return getSortValue() < that.getSortValue();
}

const UniString &Person::getSortValue() const
{
	if (m_SortValue.getI18nStr().empty()) {
		if (!m_NameRecord) {
			m_NameRecord = getNameRecord(getContactId());
		}

		const char *value = getRecordStr(m_NameRecord, m_SortProperty);
		m_SortValue = (value && *value) ? value : getName();
	}

	return m_SortValue;
}

void Person::update(contacts_record_h record)
{
	int changes = ChangedNone;
	if (!compareRecordsStr(m_Record, record, _contacts_person.display_name)) {
		changes |= ChangedName;
	}
	if (!compareRecordsStr(m_Record, record, _contacts_person.image_thumbnail_path)) {
		changes |= ChangedImage;
	}

	unsigned sortProperty = getSortProperty();
	if ((changes & ChangedName) || m_SortProperty != sortProperty) {
		changes |= updateName(record, sortProperty);
	}
	if (m_NumberRecord) {
		changes |= updateNumber(getId());
	}

	contacts_record_destroy(m_Record, true);
	m_Record = record;

	onUpdated(changes);
}

int Person::updateName(contacts_record_h record, unsigned sortProperty)
{
	int contactId = getRecordInt(record, _contacts_person.display_contact_id);
	contacts_record_h nameRecord = getNameRecord(contactId);

	int changes = ChangedNone;
	if (!Utils::safeCmp(
			getRecordStr(m_NameRecord, m_SortProperty),
			getRecordStr(nameRecord, sortProperty))) {
		changes |= ChangedSortValue;
		m_SortProperty = sortProperty;
		m_SortValue.clear();
		m_IndexLetter = getRecordStr(record, _contacts_person.display_name_index);
	}

	contacts_record_destroy(m_NameRecord, true);
	m_NameRecord = nameRecord;

	return changes;
}

int Person::updateNumber(int personId)
{
	contacts_record_h numberRecord = getNumberRecord(personId);

	int changes = ChangedNone;
	if (!compareRecordsStr(m_NumberRecord, numberRecord, _contacts_number.number)) {
		changes |= ChangedNumber;
	}

	contacts_record_destroy(m_NumberRecord, true);
	m_NumberRecord = numberRecord;

	return changes;
}
