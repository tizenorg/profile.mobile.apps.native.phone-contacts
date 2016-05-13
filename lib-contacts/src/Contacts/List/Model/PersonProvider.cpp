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

#include "Contacts/List/Model/PersonProvider.h"
#include "Contacts/List/Model/Person.h"
#include "Common/Database/Queries.h"
#include "Common/Database/RecordIterator.h"
#include "Common/Database/RecordUtils.h"
#include "Utils/Callback.h"
#include "Utils/Logger.h"

#include <algorithm>

using namespace Common::Database;
using namespace Contacts;
using namespace Contacts::Model;
using namespace Contacts::List::Model;
using namespace std::placeholders;

PersonProvider::PersonProvider(int filterType)
	: m_FilterType(filterType), m_IsFilled(false),
	  m_IsUpdateEnabled(true), m_IsUpdatePending(false)
{
	contacts_db_get_current_version(&m_DbVersion);
	contacts_db_add_changed_cb(_contacts_contact._uri,
			makeCallbackWithLastParam(&PersonProvider::onChanged), this);
	contacts_setting_add_name_display_order_changed_cb(
			makeCallbackWithLastParam(&PersonProvider::onNameFormatChanged), this);
}

PersonProvider::~PersonProvider()
{
	for (auto &&contactData : m_PersonList) {
		delete contactData;
	}

	contacts_db_remove_changed_cb(_contacts_contact._uri,
			makeCallbackWithLastParam(&PersonProvider::onChanged), this);
	contacts_setting_remove_name_display_order_changed_cb(
			makeCallbackWithLastParam(&PersonProvider::onNameFormatChanged), this);
}

const PersonProvider::DataList &PersonProvider::getDataList()
{
	if (!m_IsFilled) {
		contacts_list_h list = getPersonList();
		for (auto &&record : makeRange(list)) {
			m_PersonList.push_back(createPerson(record));
		}
		contacts_list_destroy(list, false);
		m_IsFilled = true;
	}
	return m_PersonList;
}

void PersonProvider::clearDataList()
{
	for (auto &&contactData : m_PersonList) {
		delete contactData;
	}
	m_PersonList.clear();
	m_IsFilled = false;
}

void PersonProvider::setUpdateMode(bool isEnabled)
{
	m_IsUpdateEnabled = isEnabled;
	if (m_IsUpdateEnabled && m_IsUpdatePending) {
		m_IsUpdatePending = false;
		updatePersonList();
	}
}

Person *PersonProvider::createPerson(contacts_record_h record)
{
	return new Person(record);
}

contacts_filter_h PersonProvider::getFilter() const
{
	contacts_filter_h filter = nullptr;
	if (m_FilterType != FilterNone) {
		contacts_filter_create(_contacts_person._uri, &filter);
		bool emptyFilter = true;

		if (m_FilterType & FilterNumber) {
			contacts_filter_add_bool(filter, _contacts_person.has_phonenumber, true);
			emptyFilter = false;
		}

		if (m_FilterType & FilterEmail) {
			if (!emptyFilter) {
				contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_OR);
			}
			contacts_filter_add_bool(filter, _contacts_person.has_email, true);
		}
	}

	return filter;
}

contacts_query_h PersonProvider::getQuery() const
{
	static unsigned projection[] = {
		_contacts_person.id,
		_contacts_person.display_name,
		_contacts_person.display_name_index,
		_contacts_person.display_contact_id,
		_contacts_person.image_thumbnail_path
	};

	contacts_query_h query = nullptr;
	contacts_query_create(_contacts_person._uri, &query);
	contacts_query_set_projection(query, projection, Utils::count(projection));

	return query;
}

contacts_list_h PersonProvider::getPersonList() const
{
	contacts_list_h list = nullptr;
	contacts_query_h query = getQuery();
	if (query) {
		contacts_filter_h filter = getFilter();
		if (filter) {
			contacts_query_set_filter(query, filter);
		}

		contacts_db_get_records_with_query(query, 0, 0, &list);
		contacts_query_destroy(query);
		contacts_filter_destroy(filter);
	}
	return list;
}

contacts_record_h PersonProvider::getPersonRecord(int id, IdType idType) const
{
	contacts_query_h query = getQuery();
	contacts_filter_h filter = getFilter();
	if (!filter) {
		contacts_filter_create(_contacts_person._uri, &filter);
	} else {
		contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND);
	}
	contacts_filter_add_int(filter, getIdProperty(idType), CONTACTS_MATCH_EQUAL, id);
	contacts_query_set_filter(query, filter);

	contacts_list_h list = nullptr;
	int err = contacts_db_get_records_with_query(query, 0, 1, &list);
	WARN_IF_ERR(err, "contacts_db_get_records_with_query() failed.");

	contacts_record_h record = nullptr;
	contacts_list_get_current_record_p(list, &record);

	contacts_query_destroy(query);
	contacts_filter_destroy(filter);
	contacts_list_destroy(list, false);

	return record;
}

bool PersonProvider::insertPerson(int id, IdType idType)
{
	contacts_record_h record = getPersonRecord(id, idType);
	if (record) {
		m_PersonList.push_back(createPerson(record));
		onInserted(*m_PersonList.back());
		return true;
	}

	return false;
}

bool PersonProvider::updatePerson(DataList::const_iterator personIt)
{
	Person *person = static_cast<Person *>(*personIt);
	contacts_record_h record = getPersonRecord(person->getId(), PersonId);

	if (record) {
		person->update(record);
		return true;
	}

	return false;
}

void PersonProvider::deletePerson(DataList::const_iterator personIt)
{
	Person *person = static_cast<Person *>(*personIt);
	m_PersonList.erase(personIt);
	person->onDeleted();
	delete person;
}

PersonProvider::DataList::const_iterator PersonProvider::findPerson(int id, IdType idType)
{
	int propId = getIdProperty(idType);
	return std::find_if(m_PersonList.begin(), m_PersonList.end(),
		[id, propId](ContactData *contactData) {
			Person *person = static_cast<Person *>(contactData);
			return getRecordInt(person->getRecord(), propId) == id;
		}
	);
}

int PersonProvider::getIdProperty(IdType idType)
{
	return idType == PersonId ? _contacts_person.id : _contacts_person.display_contact_id;
}

void PersonProvider::updatePersonList()
{
	contacts_list_h changes = nullptr;
	contacts_db_get_changes_by_version(_contacts_contact_updated_info._uri, 0,
			m_DbVersion, &changes, &m_DbVersion);

	for (auto &&record : makeRange(changes)) {
		int changeType = getRecordInt(record, _contacts_contact_updated_info.type);
		int contactId = getRecordInt(record, _contacts_contact_updated_info.contact_id);

		switch (changeType) {
			case CONTACTS_CHANGE_INSERTED:
			{
				insertPerson(contactId, ContactId);
				break;
			}
			case CONTACTS_CHANGE_UPDATED:
			{
				int personId = getPersonId(contactId);
				auto personIt = findPerson(personId, PersonId);
				if (personIt != m_PersonList.end()) {
					if (!updatePerson(personIt)) {
						deletePerson(personIt);
					}
				} else {
					insertPerson(personId, PersonId);
				}
				break;
			}
			case CONTACTS_CHANGE_DELETED:
			{
				auto personIt = findPerson(contactId, ContactId);
				if (personIt != m_PersonList.end()) {
					deletePerson(personIt);
				}
				break;
			}
		}
	}

	contacts_list_destroy(changes, true);
}

void PersonProvider::onChanged(const char *uri)
{
	if (m_IsUpdateEnabled) {
		updatePersonList();
	} else {
		m_IsUpdatePending = true;
	}
}

void PersonProvider::onNameFormatChanged(contacts_name_display_order_e order)
{
	for (auto &&contactData : m_PersonList) {
		Person *person = static_cast<Person *>(contactData);
		contacts_record_h record = nullptr;

		contacts_db_get_record(_contacts_person._uri, person->getId(), &record);
		person->update(record);
	}
}
