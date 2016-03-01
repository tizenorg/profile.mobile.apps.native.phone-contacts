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
#include "Utils/UniString.h"

#include <cstring>

using namespace Contacts::List::Model;
using namespace Utils;
using namespace std::placeholders;

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

	const char *getPersonValue(contacts_record_h record, contacts_person_property_e personProperty)
	{
		static struct
		{
			const char *uri;
			unsigned int value;
		} properties[] {
			/* CONTACTS_PERSON_PROPERTY_NAME_CONTACT = */
				{ _contacts_person._uri, _contacts_person.display_name },
			/* CONTACTS_PERSON_PROPERTY_NUMBER       = */
				{ _contacts_number._uri, _contacts_number.number },
			/* CONTACTS_PERSON_PROPERTY_EMAIL        = */
				{ _contacts_email._uri,  _contacts_email.email },
			/* CONTACTS_PERSON_PROPERTY_IMAGE        = */
				{ _contacts_person._uri, _contacts_person.image_thumbnail_path }
		};

		char *strValue = nullptr;

		if (personProperty == CONTACTS_PERSON_PROPERTY_NUMBER
				|| personProperty == CONTACTS_PERSON_PROPERTY_EMAIL) {
			int personId = 0;
			contacts_record_get_int(record, _contacts_person.id, &personId);

			int tableId = 0;
			contacts_person_get_default_property(personProperty, personId, &tableId);

			contacts_db_get_record(properties[personProperty].uri, tableId, &record);
		}

		contacts_record_get_str_p(record, properties[personProperty].value, &strValue);

		return strValue;
	}
}

Person::Person(contacts_record_h record)
	: ContactData(TypePerson), m_PersonRecord(record), m_ContactRecord(nullptr)
{
	init();
	subscribeToDb();
}

Person::~Person()
{
	contacts_record_destroy(m_PersonRecord, true);
	contacts_record_destroy(m_ContactRecord, true);

	unsubscribeFromDb();
}

int Person::getId() const
{
	int id = 0;
	contacts_record_get_int(m_PersonRecord, _contacts_person.id, &id);
	return id;
}

const char *Person::getName() const
{
	return getPersonValue(m_PersonRecord, CONTACTS_PERSON_PROPERTY_NAME_CONTACT);
}

const char *Person::getNumber() const
{
	return getPersonValue(m_PersonRecord, CONTACTS_PERSON_PROPERTY_NUMBER);
}

const char *Person::getImagePath() const
{
	return getPersonValue(m_PersonRecord, CONTACTS_PERSON_PROPERTY_IMAGE);
}

bool Person::compare(const char *str)
{
	return strstr(getName(), str); //Todo: Compare unicode strings
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

void Person::setRecord(contacts_record_h record)
{
	clear();
	m_PersonRecord = record;
	init();
}

bool Person::operator<(const Person &that) const
{
	return getSortValue() < that.getSortValue();
}

bool Person::operator!=(const Person &that) const
{
	return getSortValue() != that.getSortValue();
}

void Person::init()
{
	int contactId = 0;
	contacts_record_get_int(m_PersonRecord, _contacts_person.display_contact_id, &contactId);
	contacts_db_get_record(_contacts_contact._uri, contactId, &m_ContactRecord);

	char *indexLetter = nullptr;
	contacts_record_get_str_p(m_PersonRecord, _contacts_person.display_name_index, &indexLetter);
	m_IndexLetter = indexLetter;

	m_ContactIds = ::getContactIds(getId());
}

void Person::clear()
{
	contacts_record_destroy(m_PersonRecord, true);
	m_PersonRecord = nullptr;
	contacts_record_destroy(m_ContactRecord, true);
	m_ContactRecord = nullptr;

	m_IndexLetter.clear();
	m_SortValue.clear();
	m_ContactIds.clear();
	m_Handles.clear();
}

int Person::update(contacts_record_h record)
{
	const char *name = getPersonValue(record, CONTACTS_PERSON_PROPERTY_NAME_CONTACT);
	const char *number = getPersonValue(record, CONTACTS_PERSON_PROPERTY_NUMBER);
	const char *imagePath = getPersonValue(record, CONTACTS_PERSON_PROPERTY_IMAGE);

	auto changes = getChanges(name, number, imagePath);
	setRecord(record);

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
	contacts_record_get_child_record_at_p(m_ContactRecord, _contacts_contact.name, 0, &nameRecord);

	char *sortValue = nullptr;
	contacts_record_get_str_p(nameRecord, sortField, &sortValue);

	return sortValue;
}

void Person::subscribeToDb()
{
	for (auto &&id : m_ContactIds) {
		auto handle = DbChangeObserver::getInstance()->addCallback(id,
			std::bind(&Person::onChanged, this, _1, _2));

		m_Handles.push_back(handle);
	}
}

void Person::unsubscribeFromDb()
{
	for (size_t i = 0; i <  m_ContactIds.size(); ++i) {
		DbChangeObserver::getInstance()->removeCallback(m_ContactIds[i], m_Handles[i]);
	}
}

int Person::getChanges(const char *name, const char *number, const char *imagePath)
{
	int changes = ChangedNone;

	auto collectChanges = [&changes](const char *str1, const char *str2, ChangedInfo changedValue) {
		if (((bool)str1 != (bool)str2)
				|| (str1 && str2 && strcmp(str1, str2) != 0)) {
			changes |= changedValue;
		}
	};

	collectChanges(name, getName(), ChangedName);
	collectChanges(number, getNumber(), ChangedNumber);
	collectChanges(imagePath, getImagePath(), ChangedImage);

	return changes;
}

void Person::onChanged(int id, contacts_changed_e changeType)
{
	if (changeType == CONTACTS_CHANGE_UPDATED) {
		contacts_record_h record = nullptr;//Todo: Retrieve person record, using contactId
		if (record) {
			int changes = update(record);
			onUpdated(changes);
		} else {
			onDeleted();
		}
	} else if (changeType == CONTACTS_CHANGE_DELETED) {
		onDeleted();
	}
}
