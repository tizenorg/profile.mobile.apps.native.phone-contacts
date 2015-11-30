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

#include "Contacts/List/Model/ContactProvider.h"
#include "Contacts/Utils.h"
#include "Utils/Callback.h"

using namespace Contacts::List::Model;

namespace
{
	contacts_filter_h getProviderFilter(ContactProvider::FilterType filterType)
	{
		bool emptyFilter = true;
		contacts_filter_h filter = nullptr;
		contacts_filter_create(_contacts_person._uri, &filter);

		if (filterType != ContactProvider::FilterNone) {
			if (filterType & ContactProvider::FilterNumber) {
				contacts_filter_add_bool(filter, _contacts_person.has_phonenumber, true);
				emptyFilter = false;
			}

			if (filterType & ContactProvider::FilterEmail) {
				if (!emptyFilter) {
					contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND);
				}
				contacts_filter_add_bool(filter, _contacts_person.has_email, true);
			}
		}

		return filter;
	}

	contacts_record_h getPersonRecord(int contactId, ContactProvider::FilterType filterType)
	{
		contacts_filter_h filter = getProviderFilter(filterType);
		contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND);
		contacts_filter_add_int(filter, _contacts_person.display_contact_id, CONTACTS_MATCH_EQUAL, contactId);

		contacts_query_h query = nullptr;
		contacts_query_create(_contacts_person._uri, &query);
		contacts_query_set_filter(query, filter);

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

ContactProvider::ContactProvider(ContactProvider::FilterType filterType)
	: m_ListFilterType(filterType)
{
	contacts_db_get_current_version(&m_DbVersion);
	contacts_db_add_changed_cb(_contacts_person._uri, makeCallbackWithLastParam(&ContactProvider::onChanged), this);
}

contacts_list_h ContactProvider::getContactsList() const
{
	contacts_list_h list = nullptr;
	contacts_query_h query = nullptr;
	unsigned projection[] = {
		_contacts_person.display_name,
		_contacts_person.image_thumbnail_path,
		_contacts_person.display_name_index
	};

	contacts_query_create(_contacts_person._uri, &query);

	contacts_filter_h contactsFilter = getProviderFilter(m_ListFilterType);
	contacts_query_set_filter(query, contactsFilter);

	contacts_query_set_projection(query, projection, sizeof(projection) / sizeof(*projection));
	contacts_query_set_sort(query, _contacts_person.display_name, true);//Todo: Get sort option from settings
	contacts_db_get_records_with_query(query, 0, 0, &list);

	contacts_filter_destroy(contactsFilter);
	contacts_query_destroy(query);

	return list;
}

void ContactProvider::setChangeCallback(int id, ChangeCallback callback)
{
	m_ChangeCallbacks.insert({ id, std::move(callback) });
}

void ContactProvider::unsetChangeCallback(int id)
{
	m_ChangeCallbacks.erase(id);
}

void ContactProvider::setInsertCallback(InsertCallback callback)
{
	m_CreateCallback = std::move(callback);
}

void ContactProvider::unsetInsertCallback()
{
	m_CreateCallback = nullptr;
}

void ContactProvider::onChanged(const char *viewUri)
{
	contacts_list_h changes = nullptr;
	contacts_db_get_changes_by_version(_contacts_contact_updated_info._uri, 0, m_DbVersion, &changes, &m_DbVersion);

	contacts_record_h record = nullptr;
	CONTACTS_LIST_FOREACH(changes, record) {
		int contactId = 0;
		int changeType = -1;

		contacts_record_get_int(record, _contacts_contact_updated_info.contact_id, &contactId);
		contacts_record_get_int(record, _contacts_contact_updated_info.type, &changeType);

		contacts_record_h personRecord = getPersonRecord(contactId, m_ListFilterType);
		notify(static_cast<contacts_changed_e>(changeType), personRecord);
		contacts_record_destroy(personRecord, true);
	}

	contacts_list_destroy(changes, true);
}

void ContactProvider::notify(contacts_changed_e changeType, contacts_record_h record)
{
	int id = 0;
	contacts_record_get_int(record, _contacts_person.id, &id);

	if (changeType == CONTACTS_CHANGE_INSERTED && m_CreateCallback) {
		m_CreateCallback(record);
	} else {
		auto it = m_ChangeCallbacks.find(id);
		if (it != m_ChangeCallbacks.end()) {
			it->second(it->first, changeType);
		}
	}
}
