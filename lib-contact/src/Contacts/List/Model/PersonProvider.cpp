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
#include "Utils/Logger.h"

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


	contacts_filter_h prepareModeTypeFilter(PersonProvider::Mode modeType)
	{
		contacts_filter_h filter = nullptr;

		if (modeType != PersonProvider::ModeAll) {
			contacts_filter_create(_contacts_person._uri, &filter);

			if (modeType == PersonProvider::ModeFavorites) {
				contacts_filter_add_bool(filter, _contacts_person.is_favorite, true);
			} else if (modeType == PersonProvider::ModeMFC) {
				//TODO Implement
			}
		}

		return filter;
	}

	contacts_filter_h prepareFilterTypeFilter(PersonProvider::FilterType filterType)
	{
		contacts_filter_h filter = nullptr;

		if (filterType != PersonProvider::FilterNone) {
			contacts_filter_create(_contacts_person._uri, &filter);
			bool emptyFilter = true;

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

	contacts_filter_h getProviderFilter(PersonProvider::Mode modeType, PersonProvider::FilterType filterType)
	{
		contacts_filter_h result = nullptr;
		contacts_filter_h modeTypeFilter = prepareModeTypeFilter(modeType);
		contacts_filter_h filterTypeFilter = prepareFilterTypeFilter(filterType);

		if (modeTypeFilter) {
			if (filterTypeFilter) {
				contacts_filter_add_operator(modeTypeFilter, CONTACTS_FILTER_OPERATOR_AND);
				contacts_filter_add_filter(modeTypeFilter, filterTypeFilter);
			}
			result = modeTypeFilter;
		} else if (filterTypeFilter) {
			result = filterTypeFilter;
		}

		return result;
	}

	contacts_list_h getPersonList(PersonProvider::Mode modeType, PersonProvider::FilterType filterType)
	{
		contacts_query_h query = nullptr;
		contacts_query_create(_contacts_person._uri, &query);

		contacts_filter_h filter = getProviderFilter(modeType, filterType);
		if (filter) {
			contacts_query_set_filter(query, filter);
		}

		contacts_query_set_projection(query, projection, Utils::count(projection));
		if (PersonProvider::ModeFavorites == modeType) {
			contacts_query_set_sort(query, _contacts_person.favorite_priority, true);
		}

		contacts_list_h list = nullptr;
		contacts_db_get_records_with_query(query, 0, 0, &list);

		contacts_query_destroy(query);
		contacts_filter_destroy(filter);

		return list;
	}

	contacts_record_h getPersonRecord(int contactId, PersonProvider::Mode modeType, PersonProvider::FilterType filterType)
	{
		contacts_query_h query = nullptr;
		contacts_query_create(_contacts_person._uri, &query);

		contacts_filter_h filter = getProviderFilter(modeType, filterType);
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
}

PersonProvider::PersonProvider(Mode modeType, FilterType filterType)
	: m_Mode(modeType), m_FilterType(filterType)
{
	if (m_Mode == ModeMFC) {
		//TODO Implement support of ModeMFC
		ERR("ModeTypeMFC is not supported");
		m_Mode = ModeAll;
	}
}

PersonProvider::~PersonProvider()
{
	unsetInsertCallback();

	for (auto &&callback : m_ChangeCallbacks) {
		DbChangeObserver::getInstance()->removeCallback(callback.first, callback.second.second);
	}
}

PersonList PersonProvider::getPersonList() const
{
	PersonList personList;
	contacts_list_h list = ::getPersonList(m_Mode, m_FilterType);

	contacts_record_h record = nullptr;
	CONTACTS_LIST_FOREACH(list, record) {
		personList.push_back(PersonPtr(new Person(record)));
	}

	contacts_list_destroy(list, false);

	return personList;
}

void PersonProvider::setInsertCallback(InsertCallback callback)
{
	auto handle = DbChangeObserver::getInstance()->addCallback(
			std::bind(&PersonProvider::onPersonInserted, this, _1, _2));
	m_InsertCallback = { std::move(callback), std::move(handle) };
}

void PersonProvider::unsetInsertCallback()
{
	if (m_InsertCallback.first) {
		auto handle = m_InsertCallback.second;
		DbChangeObserver::getInstance()->removeCallback(handle);
		m_InsertCallback.first = nullptr;
	}
}

void PersonProvider::setChangeCallback(const Person &person, ChangeCallback callback)
{
	auto addCallback = [this](int id, ChangeCallback callback) {
		auto handle = DbChangeObserver::getInstance()->addCallback(id,
			std::bind(&PersonProvider::onPersonChanged, this, _1, _2));

		m_ChangeCallbacks.insert({ id, { std::move(callback), std::move(handle) } });
	};

	auto ids = person.getContactIds();
	auto it = ids.begin();

	for (; it < ids.end() - 1; ++it) {
		addCallback(*it, callback);
	}

	addCallback(*it, std::move(callback));
}

void PersonProvider::unsetChangeCallback(const Person &person)
{
	for (auto &&id: person.getContactIds()) {
		auto it = m_ChangeCallbacks.find(id);
		if (it != m_ChangeCallbacks.end()) {
			auto handle = it->second.second;
			DbChangeObserver::getInstance()->removeCallback(id, handle);

			m_ChangeCallbacks.erase(it);
		}
	}
}

void PersonProvider::onPersonInserted(int id, contacts_changed_e changeType)
{
	if (changeType == CONTACTS_CHANGE_INSERTED) {
		auto callback = m_InsertCallback.first;
		if (callback) {
			contacts_record_h record = getPersonRecord(id, m_Mode, m_FilterType);
			callback(PersonPtr(new Person(record)));
		}
	}
}

void PersonProvider::onPersonChanged(int id, contacts_changed_e changeType)
{
	auto it = m_ChangeCallbacks.find(id);
	if (it != m_ChangeCallbacks.end()) {
		auto callback = it->second.first;

		if (callback) {
			if (changeType == CONTACTS_CHANGE_UPDATED) {
				contacts_record_h record = getPersonRecord(id, m_Mode, m_FilterType);
				callback(PersonPtr(new Person(record)), changeType);
			} else if (changeType == CONTACTS_CHANGE_DELETED) {
				callback(nullptr, changeType);
			}
		}
	}
}
