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

#include "Contacts/List/Model/Contact.h"
#include "Utils/UniString.h"

using namespace Contacts::List::Model;
using namespace Utils;

namespace
{
	unsigned getSortProperty(contacts_name_sorting_order_e order)
	{
		return (order == CONTACTS_NAME_SORTING_ORDER_FIRSTLAST
				? _contacts_name.first : _contacts_name.last);
	}
}

Contact::Contact(contacts_record_h record)
	: m_PersonRecord(record), m_ContactRecord(nullptr), m_SortValue(nullptr)
{
	int contactId = 0;
	contacts_record_get_int(m_PersonRecord, _contacts_person.display_contact_id, &contactId);

	//Todo: Store only handle for contact name while Contact list view will be implemented
	contacts_db_get_record(_contacts_contact._uri, contactId, &m_ContactRecord);
}

Contact::~Contact()
{
	contacts_record_destroy(m_PersonRecord, true);
	contacts_record_destroy(m_ContactRecord, true);

	delete m_SortValue;
}

bool Contact::operator<(const Contact &that) const
{
	return getSortValue() < that.getSortValue();
}

bool Contact::operator==(const Contact &that) const
{
	return getSortValue() == that.getSortValue();
}

bool Contact::operator!=(const Contact &that) const
{
	return getSortValue() != that.getSortValue();
}

int Contact::getPersonId() const
{
	int id = 0;
	contacts_record_get_int(m_PersonRecord, _contacts_person.id, &id);
	return id;
}

const char *Contact::getIndexLetter() const
{
	char *index = nullptr;
	contacts_record_get_str_p(m_PersonRecord, _contacts_person.display_name_index, &index);
	return index;
}

const char *Contact::getName() const
{
	char *name = nullptr;
	contacts_record_get_str_p(m_PersonRecord, _contacts_person.display_name, &name);
	return name;
}

const char *Contact::getImagePath() const
{
	char *path = nullptr;
	contacts_record_get_str_p(m_PersonRecord, _contacts_person.image_thumbnail_path, &path);
	return path;
}

const contacts_record_h Contact::getRecord() const
{
	return m_PersonRecord;
}

const UniString &Contact::getSortValue() const
{
	if (!m_SortValue) {
		m_SortValue = new UniString(getDbSortValue());
	}

	return *m_SortValue;
}

const char *Contact::getDbSortValue() const
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
