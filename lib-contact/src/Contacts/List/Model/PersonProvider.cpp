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
#include "Contacts/Utils.h"

using namespace Contacts;
using namespace Contacts::List::Model;
using namespace std::placeholders;

namespace
{
	unsigned projection[] = {
		_contacts_person.id,
		_contacts_person.display_name,
		_contacts_person.display_name_index,
		_contacts_person.display_contact_id,
		_contacts_person.image_thumbnail_path,
		_contacts_person.is_favorite,
		_contacts_person.favorite_priority
	};

	contacts_filter_h getProviderFilter(int filterType)
	{
		contacts_filter_h filter = nullptr;

		if (filterType != FilterNone) {
			contacts_filter_create(_contacts_person._uri, &filter);
			bool emptyFilter = true;

			if (filterType & FilterNumber) {
				contacts_filter_add_bool(filter, _contacts_person.has_phonenumber, true);
				emptyFilter = false;
			}

			if (filterType & FilterEmail) {
				if (!emptyFilter) {
					contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND);
				}
				contacts_filter_add_bool(filter, _contacts_person.has_email, true);
			}
		}

		return filter;
	}

	contacts_list_h getPersonList(int filterType)
	{
		contacts_query_h query = nullptr;
		contacts_query_create(_contacts_person._uri, &query);

		contacts_filter_h filter = getProviderFilter(filterType);
		if (filter) {
			contacts_query_set_filter(query, filter);
		}

		contacts_query_set_projection(query, projection, Utils::count(projection));

		contacts_list_h list = nullptr;
		contacts_db_get_records_with_query(query, 0, 0, &list);

		contacts_query_destroy(query);
		contacts_filter_destroy(filter);

		return list;
	}
}

PersonProvider::PersonProvider(int filterType)
	: m_FilterType(filterType)
{
	m_Handle = DbChangeObserver::getInstance()->addCallback(
			std::bind(&PersonProvider::onPersonInserted, this, _1, _2));
}

PersonProvider::~PersonProvider()
{
	DbChangeObserver::getInstance()->removeCallback(m_Handle);

	for(auto &&item : m_PersonList) {
		auto person = static_cast<Person *>(item);
		unsetChangedCallback(*person);
		delete person;
	}
}

const ContactDataList &PersonProvider::getContactDataList()
{
	contacts_list_h list = ::getPersonList(m_FilterType);

	contacts_record_h record = nullptr;
	CONTACTS_LIST_FOREACH(list, record) {
		insertPerson(record);
	}

	contacts_list_destroy(list, false);

	return m_PersonList;
}

contacts_record_h PersonProvider::getFilteredRecord(int contactId)
{
	contacts_query_h query = nullptr;
	contacts_query_create(_contacts_person._uri, &query);

	contacts_filter_h filter = getProviderFilter(m_FilterType);
	if (filter) {
		contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND);
	} else {
		contacts_filter_create(_contacts_person._uri, &filter);
	}

	contacts_filter_add_int(filter, _contacts_person.display_contact_id, CONTACTS_MATCH_EQUAL, contactId);
	contacts_query_set_filter(query, filter);
	contacts_query_set_projection(query, projection, Utils::count(projection));

	contacts_list_h list = nullptr;
	contacts_db_get_records_with_query(query, 0, 1, &list);

	contacts_record_h record = nullptr;
	contacts_list_get_current_record_p(list, &record);

	contacts_list_destroy(list, false);
	contacts_query_destroy(query);
	contacts_filter_destroy(filter);

	return record;
}

void PersonProvider::insertPerson(contacts_record_h record)
{
	m_PersonList.push_back(new Person(record));
	setChangedCallback(--m_PersonList.end());
}

void PersonProvider::setChangedCallback(ContactDataList::iterator personIt)
{
	auto person = static_cast<Person *>(*personIt);

	for (auto &&id : person->getContactIds()) {
		auto handle = DbChangeObserver::getInstance()->addCallback(id,
			std::bind(&PersonProvider::onPersonChanged, this, personIt, _1, _2));

		person->m_Handles.push_back(handle);
	}
}

void PersonProvider::updateChangedCallback(ContactDataList::iterator personIt)
{
	auto person = static_cast<Person *>(*personIt);

	unsetChangedCallback(*person);
	person->m_Handles.clear();
	setChangedCallback(personIt);
}

void PersonProvider::unsetChangedCallback(const Person &person)
{
	auto &contactIds = person.getContactIds();
	for (size_t i = 0; i <  contactIds.size(); ++i) {
		DbChangeObserver::getInstance()->removeCallback(contactIds[i], person.m_Handles[i]);
	}
}

void PersonProvider::onPersonInserted(int id, contacts_changed_e changeType)
{
	if (changeType == CONTACTS_CHANGE_INSERTED) {
		auto record = getFilteredRecord(id);
		if (record) {
			insertPerson(record);
			onInserted(*m_PersonList.back());
		}
	}
}

void PersonProvider::onPersonChanged(ContactDataList::iterator it, int id, contacts_changed_e changeType)
{
	auto person = static_cast<Person *>(*it);

	if (changeType == CONTACTS_CHANGE_UPDATED) {
		contacts_record_h record = getFilteredRecord(id);
		if (record) {
			int changes = person->updatePerson(record);
			updateChangedCallback(it);
			person->onUpdated(changes);

			return;
		}
	}

	unsetChangedCallback(*person);
	person->onDeleted();

	delete person;
	m_PersonList.erase(it);
}
