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
	: m_FilterType(filterType), m_DbVersion(-1), m_IsFilled(false),
	  m_IsUpdateEnabled(true), m_IsUpdatePending(false)
{
}

PersonProvider::~PersonProvider()
{
	unsubscribe();

	for (auto &&contactData : m_PersonList) {
		delete contactData;
	}
}

int PersonProvider::getFilterType() const
{
	return m_FilterType;
}

const PersonProvider::DataList &PersonProvider::getDataList()
{
	if (!m_IsFilled) {
		contacts_list_h persons = getPersonList();
		for (auto &&record : makeRange(persons)) {
			m_PersonList.push_back(createPerson(record));
		}
		contacts_list_destroy(persons, false);
		addContacts();

		subscribe();
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

	unsubscribe();
	m_IsFilled = false;
}

void PersonProvider::reload()
{
	/*
	 * This done to support update functionality of Mfc.
	 * The PersonProvider's methods are used because list is fully cleared and
	 * the new list is get using overridden getPersonList method.
	 *
	 * If some of inherited classes will keep own person list, this code should be updated.
	 */
	while (!getDataList().empty()) {
		PersonProvider::deletePerson(getDataList().begin());
	}

	resetDbVersion();

	contacts_list_h list = getPersonList();
	for (auto &&record : makeRange(list)) {
		PersonProvider::insertPerson(record);
	}

	contacts_list_destroy(list, true);
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

void PersonProvider::insertPerson(contacts_record_h personRecord)
{
	if (personRecord) {
		Person *person = createPerson(personRecord);

		contacts_list_h contacts = getRecordList(_contacts_contact._uri, _contacts_contact.person_id,
				person->getId());

		for (auto &&contact : makeRange(contacts)) {
			person->addContact(contact);
		}

		m_PersonList.push_back(person);
		onInserted(*m_PersonList.back());

		contacts_list_destroy(contacts, false);
	}
}

bool PersonProvider::updatePerson(DataList::const_iterator personIt, contacts_record_h personRecord, int contactId)
{
	Person *person = static_cast<Person *>(*personIt);
	if (personRecord) {
		contacts_record_h contactRecord = nullptr;
		contacts_db_get_record(_contacts_contact._uri, contactId, &contactRecord);
		int displayContactId = getRecordInt(personRecord, _contacts_person.display_contact_id);

		person->addContact(contactRecord, displayContactId == getRecordInt(contactRecord, _contacts_contact.id));
		person->update(personRecord);
	} else {
		deletePerson(personIt);
	}

	return personRecord != nullptr;
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
	if (idType == PersonId) {
		return std::find_if(m_PersonList.begin(), m_PersonList.end(),
			[id](::Model::DataItem *data) {
				Person *person = static_cast<Person *>(data);
				return person->getId() == id;
			}
		);
	} else {
		return std::find_if(m_PersonList.begin(), m_PersonList.end(),
			[id](::Model::DataItem *data) {
				Person *person = static_cast<Person *>(data);
				return std::find_if(person->m_ContactRecords.begin(), person->m_ContactRecords.end(),
					[id](contacts_record_h contactRecord) {
						return getRecordInt(contactRecord, _contacts_contact.id) == id;
					}
				) != person->m_ContactRecords.end();
			}
		);
	}
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
				insertPerson(getPersonRecord(contactId, ContactId));
				break;
			}
			case CONTACTS_CHANGE_UPDATED:
			{
				int personId = getPersonId(contactId);
				contacts_record_h personRecord = getPersonRecord(contactId, ContactId);
				auto personIt = findPerson(personId, PersonId);

				if (personIt != m_PersonList.end()) {
					updatePerson(personIt, personRecord, contactId);
				} else {
					insertPerson(personRecord);
				}
				break;
			}
			case CONTACTS_CHANGE_DELETED:
				removeContact(contactId);
				break;
		}
	}

	contacts_list_destroy(changes, true);
	onUpdateFinished();
}


void PersonProvider::addContacts()
{
	contacts_list_h contacts = nullptr;
	contacts_db_get_all_records(_contacts_contact._uri, 0, 0, &contacts);
	for (auto &&record : makeRange(contacts)) {
		auto it = findPerson(getRecordInt(record, _contacts_contact.person_id), PersonId);
		if (it != m_PersonList.end()) {
			Person *person = static_cast<Person *>(*it);
			person->addContact(record);
		} else {
			contacts_record_destroy(record, true);
		}
	}
	contacts_list_destroy(contacts, false);
}

void PersonProvider::removeContact(int contactId)
{
	auto personIt = findPerson(contactId, ContactId);
	if (personIt != m_PersonList.end()) {
		Person &person = static_cast<Person &>(**personIt);
		person.removeContact(contactId);
		if (!person.getContactCount()) {
			deletePerson(personIt);
		}
	}
}

void PersonProvider::subscribe()
{
	resetDbVersion();

	contacts_db_add_changed_cb(_contacts_contact._uri,
			makeCallbackWithLastParam(&PersonProvider::onChanged), this);
	contacts_setting_add_name_display_order_changed_cb(
			&PersonProvider::onNameFormatChanged, this);
	contacts_setting_add_name_sorting_order_changed_cb(
			&PersonProvider::onSortOrderChanged, this);
}

void PersonProvider::unsubscribe()
{
	contacts_db_remove_changed_cb(_contacts_contact._uri,
			makeCallbackWithLastParam(&PersonProvider::onChanged), this);
	contacts_setting_remove_name_display_order_changed_cb(
			&PersonProvider::onNameFormatChanged, this);
	contacts_setting_remove_name_sorting_order_changed_cb(
			&PersonProvider::onSortOrderChanged, this);
}

void PersonProvider::resetDbVersion()
{
	contacts_db_get_current_version(&m_DbVersion);
}

void PersonProvider::onChanged(const char *uri)
{
	if (m_IsUpdateEnabled) {
		updatePersonList();
	} else {
		m_IsUpdatePending = true;
	}
}

void PersonProvider::onSettingsChanged()
{
	for (auto &&contactData : m_PersonList) {
		Person *person = static_cast<Person *>(contactData);
		contacts_record_h record = nullptr;

		contacts_db_get_record(_contacts_person._uri, person->getId(), &record);
		person->update(record);
	}
}

void PersonProvider::onNameFormatChanged(contacts_name_display_order_e order, void *data)
{
	PersonProvider *provider = (PersonProvider *) data;
	provider->onSettingsChanged();
}

void PersonProvider::onSortOrderChanged(contacts_name_sorting_order_e order, void *data)
{
	PersonProvider *provider = (PersonProvider *) data;
	provider->onSettingsChanged();
	provider->onUpdateFinished();
}
