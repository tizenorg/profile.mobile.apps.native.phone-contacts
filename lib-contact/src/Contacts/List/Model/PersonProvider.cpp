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
#include "Contacts/Utils.h"
#include "Utils/Callback.h"
#include "Utils/Range.h"

using namespace Contacts::List::Model;

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

	contacts_filter_h getProviderFilter(PersonProvider::FilterType filterType)
	{
		bool emptyFilter = true;
		contacts_filter_h filter = nullptr;
		contacts_filter_create(_contacts_person._uri, &filter);

		if (filterType != PersonProvider::FilterNone) {
			if (filterType & PersonProvider::FilterNumber) {
				contacts_filter_add_bool(filter, _contacts_person.has_phonenumber, true);
				emptyFilter = false;
			}

			if (filterType & PersonProvider::FilterEmail) {
				if (!emptyFilter) {
					contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND);
				}
				contacts_filter_add_bool(filter, _contacts_person.has_email, true);
			}
		}

		return filter;
	}

	contacts_list_h getPersonList(PersonProvider::FilterType filterType)
	{
		contacts_filter_h filter = getProviderFilter(filterType);

		contacts_query_h query = nullptr;
		contacts_query_create(_contacts_person._uri, &query);
		contacts_query_set_filter(query, filter);
		contacts_query_set_projection(query, projection, Utils::count(projection));

		contacts_list_h list = nullptr;
		contacts_db_get_records_with_query(query, 0, 0, &list);

		contacts_query_destroy(query);
		contacts_filter_destroy(filter);

		return list;
	}

	contacts_record_h getPersonRecord(int contactId, PersonProvider::FilterType filterType)
	{
		contacts_filter_h filter = getProviderFilter(filterType);
		contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND);
		contacts_filter_add_int(filter, _contacts_person.display_contact_id, CONTACTS_MATCH_EQUAL, contactId);

		contacts_query_h query = nullptr;
		contacts_query_create(_contacts_person._uri, &query);
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
}

PersonProvider::PersonProvider(PersonProvider::FilterType filterType)
	: m_ListFilterType(filterType)
{
	contacts_db_get_current_version(&m_DbVersion);
	contacts_db_add_changed_cb(_contacts_person._uri, makeCallbackWithLastParam(&PersonProvider::onChanged), this);
}

PersonList PersonProvider::getPersonList() const
{
	PersonList personList;
	contacts_list_h list = ::getPersonList(m_ListFilterType);

	contacts_record_h record = nullptr;
	CONTACTS_LIST_FOREACH(list, record) {
		personList.push_back(PersonPtr(new Person(record)));
	}

	contacts_list_destroy(list, false);

	return personList;
}

void PersonProvider::setChangeCallback(const Person &person, ChangeCallback callback)
{
	auto ids = person.getContactIds();

	auto it = ids.begin();
	for (; it != ids.end(); ++it) {
		m_ChangeCallbacks.insert({ *it, callback });
	}
	m_ChangeCallbacks.insert({ *it, std::move(callback) });
}

void PersonProvider::unsetChangeCallback(const Person &person)
{
	for (auto &&id: person.getContactIds()) {
		m_ChangeCallbacks.erase(id);
	}
}

void PersonProvider::setInsertCallback(InsertCallback callback)
{
	m_InsertCallback = std::move(callback);
}

void PersonProvider::unsetInsertCallback()
{
	m_InsertCallback = nullptr;
}

void PersonProvider::onChanged(const char *viewUri)
{
	contacts_list_h changes = nullptr;
	contacts_db_get_changes_by_version(_contacts_contact_updated_info._uri, 0, m_DbVersion, &changes, &m_DbVersion);

	contacts_record_h record = nullptr;
	CONTACTS_LIST_FOREACH(changes, record) {
		int contactId = 0;
		int changeType = -1;

		contacts_record_get_int(record, _contacts_contact_updated_info.contact_id, &contactId);
		contacts_record_get_int(record, _contacts_contact_updated_info.type, &changeType);

		notify(static_cast<contacts_changed_e>(changeType), contactId);
	}

	contacts_list_destroy(changes, true);
}

void PersonProvider::notify(contacts_changed_e changeType, int contactId)
{
	PersonPtr person;

	switch (changeType) {
		case CONTACTS_CHANGE_INSERTED:
			person.reset(new Person(getPersonRecord(contactId, m_ListFilterType)));
			if (m_InsertCallback) {
				m_InsertCallback(std::move(person));
			}
			break;
		case CONTACTS_CHANGE_UPDATED:
			//Todo: If will be link contact functionality, update only when display contact changed
			person.reset(new Person(getPersonRecord(contactId, m_ListFilterType)));
		case CONTACTS_CHANGE_DELETED:
		{
			auto it = m_ChangeCallbacks.find(contactId);
			if (it != m_ChangeCallbacks.end()) {
				//Todo: If will be link contact functionality, delete only when it was last contact in person
				it->second(std::move(person), changeType);
			}
		}
			break;
	}
}
