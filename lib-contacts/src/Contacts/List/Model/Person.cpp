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
#include "Common/Database/Queries.h"
#include "Common/Database/RecordUtils.h"
#include "Common/Database/RecordIterator.h"
#include "Utils/Logger.h"

#include <algorithm>

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

	contacts_record_h getDefaultNumberRecord(contacts_record_h contactRecord)
	{
		return getChildRecord(contactRecord, _contacts_contact.number,
			[](contacts_record_h record) {
				return getRecordBool(record, _contacts_number.is_default);
			});
	}
}

Person::Person(contacts_record_h record)
	: ContactData(TypePerson),
	  m_Record(record), m_DefaultContactRecord(nullptr),
	  m_SortProperty(getSortProperty())
{
	m_IndexLetter = getRecordStr(m_Record, _contacts_person.display_name_index);
}

Person::~Person()
{
	for (auto &&contact : m_ContactRecords) {
		contacts_record_destroy(contact, true);
	}
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
	return getRecordStr(getDefaultNumberRecord(m_DefaultContactRecord), _contacts_number.number);
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

const char *Person::getNickname() const
{
	contacts_record_h record  = getContactChildRecord(_contacts_contact.nickname,
		[](contacts_record_h record) {
			return getRecordStr(record, _contacts_nickname.name) != nullptr;
		}
	);

	return record ? getRecordStr(record, _contacts_nickname.name) : nullptr;
}

const char *Person::getNotes() const
{
	contacts_record_h record  = getContactChildRecord(_contacts_contact.note,
		[](contacts_record_h record) {
			return getRecordStr(record, _contacts_note.note) != nullptr;
		}
	);

	return record ? getRecordStr(record, _contacts_note.note) : nullptr;
}

contacts_record_h Person::getOrganization() const
{
	return getContactChildRecord(_contacts_contact.company,
		[](contacts_record_h record) {
			return getRecordStr(record, _contacts_company.name) != nullptr
				|| getRecordStr(record, _contacts_company.job_title) != nullptr;
		}
	);
}

const Person::ContactChildRecords Person::getAddresses() const
{
	return getContactChildRecords(_contacts_contact.address);
}

const Person::ContactChildRecords Person::getEmails() const
{
	return getContactChildRecords(_contacts_contact.email);
}

const Person::ContactChildRecords Person::getNumbers() const
{
	return getContactChildRecords(_contacts_contact.number);
}

const UniString &Person::getIndexLetter() const
{
	return m_IndexLetter;
}

bool Person::operator<(const Person &that) const
{
	return getSortValue() < that.getSortValue();
}

void Person::addContact(contacts_record_h record)
{
	if (getContactId() == getRecordInt(record, _contacts_contact.id)) {
		m_DefaultContactRecord = record;
	}

	if (!m_ContactRecords.empty()) {
		auto it = std::find_if(m_ContactRecords.begin(), m_ContactRecords.end(),
			[record](contacts_record_h contactRecord) {
				return compareRecordsInt(contactRecord, record, _contacts_contact.id);
			}
		);

		if (it != m_ContactRecords.end()) {
			*it = record;
			return;
		}
	}

	m_ContactRecords.push_back(record);
}

void Person::removeContact(int id)
{
	auto it = std::find_if(m_ContactRecords.begin(), m_ContactRecords.end(),
		[id](contacts_record_h contactRecord) {
			return getRecordInt(contactRecord, _contacts_contact.id) == id;
		}
	);

	if (it != m_ContactRecords.end()) {
		bool isDefault = *it == m_DefaultContactRecord;

		contacts_record_destroy(*it, true);
		m_ContactRecords.erase(it);

		if (isDefault && !m_ContactRecords.empty()) {
			contacts_record_destroy(m_Record, true);
			contacts_db_get_record(_contacts_person._uri, getId(), &m_Record);

			int displayContactId = getRecordInt(m_Record, _contacts_person.display_contact_id);
			std::find_if(m_ContactRecords.begin(), m_ContactRecords.end(),
				[this, displayContactId](contacts_record_h contactRecord) {
					return displayContactId == getRecordInt(contactRecord, _contacts_contact.id);
				}
			);
		}
	}
}

size_t Person::getContactCount() const
{
	return m_ContactRecords.size();
}

template <typename Pred>
contacts_record_h Person::getContactChildRecord(unsigned propertyId, Pred predicate) const
{
	contacts_record_h record = getChildRecord(m_DefaultContactRecord, propertyId);
	if (record && predicate(record)) {
		return record;
	}

	for (auto &&contact : m_ContactRecords) {
		record = getChildRecord(contact, propertyId);
		if (record && predicate(record)) {
			return record;
		}
	}

	return nullptr;
}

const Person::ContactChildRecords Person::getContactChildRecords(unsigned propertyId) const
{
	ContactChildRecords records;
	for (auto &&contact : m_ContactRecords) {
		records.push_back(makeRange(contact, propertyId));
	}

	return records;
}

const UniString &Person::getSortValue() const
{
	if (m_SortValue.getI18nStr().empty()) {
		contacts_record_h nameRecord = getChildRecord(m_DefaultContactRecord, _contacts_contact.name);
		const char *value = getRecordStr(nameRecord, m_SortProperty);
		m_SortValue = (value && *value) ? value : getName();
	}

	return m_SortValue;
}

void Person::update(contacts_record_h personRecord)
{
	int changes = ChangedNone;
	if (!compareRecordsStr(m_Record, personRecord, _contacts_person.display_name)) {
		changes |= ChangedName;
	}
	if (!compareRecordsStr(m_Record, personRecord, _contacts_person.image_thumbnail_path)) {
		changes |= ChangedImage;
	}

	unsigned sortProperty = getSortProperty();
	if ((changes & ChangedName) || m_SortProperty != sortProperty) {
		changes |= updateName(personRecord, sortProperty);
	}
	changes |= ChangedContact;

	contacts_record_destroy(m_Record, true);
	m_Record = personRecord;

	onUpdated(changes);
}

int Person::updateName(contacts_record_h record, unsigned sortProperty)
{
	int contactId = getRecordInt(record, _contacts_person.display_contact_id);
	contacts_record_h nameRecord = getNameRecord(contactId);//Todo: Get rid of this query invocation

	int changes = ChangedNone;
	if (!Utils::safeCmp(
			getRecordStr(getChildRecord(m_DefaultContactRecord, _contacts_contact.name), m_SortProperty),
			getRecordStr(nameRecord, sortProperty))) {
		changes |= ChangedSortValue;
		m_SortValue.clear();
		m_IndexLetter = getRecordStr(record, _contacts_person.display_name_index);
	}

	m_SortProperty = sortProperty;
	contacts_record_destroy(nameRecord, true);
	return changes;
}
