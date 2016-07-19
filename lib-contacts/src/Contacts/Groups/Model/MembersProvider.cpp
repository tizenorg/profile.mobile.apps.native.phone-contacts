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

#include "Contacts/Groups/Model/MembersProvider.h"

#include "Contacts/Groups/Model/Queries.h"

#include "Common/Database/RecordIterator.h"
#include "Common/Database/RecordUtils.h"
#include "Utils/Callback.h"

using namespace Common::Database;
using namespace Contacts::Groups::Model;

namespace
{
	struct FilterData {
		contacts_match_int_flag_e matchType;
		contacts_filter_operator_e operatorType;
	} filterData[] = {
		/* ModeDefault */ { CONTACTS_MATCH_EQUAL, CONTACTS_FILTER_OPERATOR_OR },
		/* ModeExclude */ { CONTACTS_MATCH_NOT_EQUAL, CONTACTS_FILTER_OPERATOR_AND }
	};
}

MembersProvider::MembersProvider(int groupId, Mode mode)
	: m_GroupId(groupId), m_Mode(mode)
{
	contacts_db_get_current_version(&m_DbVersion);
	m_GroupDbVersion = m_DbVersion;
	contacts_db_add_changed_cb(_contacts_group_relation._uri,
			makeCallbackWithLastParam(&MembersProvider::onChanged), this);
	contacts_db_add_changed_cb(_contacts_group._uri,
			makeCallbackWithLastParam(&MembersProvider::onGroupChanged), this);
}

MembersProvider::~MembersProvider()
{
	contacts_db_remove_changed_cb(_contacts_group_relation._uri,
			makeCallbackWithLastParam(&MembersProvider::onChanged), this);
	contacts_db_remove_changed_cb(_contacts_group._uri,
			makeCallbackWithLastParam(&MembersProvider::onGroupChanged), this);
}

contacts_filter_h MembersProvider::getFilter() const
{
	contacts_list_h membersList = getMembersList();
	int count = 0;
	contacts_list_get_count(membersList, &count);
	if (m_Mode == ModeDefault && !count) {
		contacts_list_destroy(membersList, true);
		return nullptr;
	}

	contacts_filter_h filter = nullptr;
	contacts_filter_create(_contacts_person._uri, &filter);
	if (m_Mode == ModeExclude) {
		contacts_filter_add_bool(filter, _contacts_person.is_favorite, false);
		contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND);
	}

	contacts_filter_h membersFilter = nullptr;
	contacts_filter_create(_contacts_person._uri, &membersFilter);
	for (auto &&record : makeRange(membersList)) {
		int personId = getRecordInt(record, _contacts_person_group_assigned.person_id);
		contacts_filter_add_int(membersFilter, _contacts_person.id, filterData[m_Mode].matchType, personId);
		contacts_filter_add_operator(membersFilter, filterData[m_Mode].operatorType);
	}
	contacts_filter_add_filter(filter, membersFilter);

	contacts_list_destroy(membersList, true);
	contacts_filter_destroy(membersFilter);
	return filter;
}

contacts_list_h MembersProvider::getPersonList() const
{
	contacts_filter_h filter = getFilter();
	if (!filter) {
		return nullptr;
	}

	contacts_query_h query = nullptr;
	contacts_query_create(_contacts_person._uri, &query);
	contacts_query_set_filter(query, filter);

	contacts_list_h list = nullptr;
	contacts_db_get_records_with_query(query, 0, 0, &list);

	contacts_filter_destroy(filter);
	contacts_query_destroy(query);
	return list;
}

contacts_record_h MembersProvider::getPersonRecord(int id, IdType idType) const
{
	contacts_filter_h filter = getFilter();
	if (!filter) {
		return nullptr;
	}
	contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND);
	contacts_filter_add_int(filter, getIdProperty(idType), CONTACTS_MATCH_EQUAL, id);

	contacts_query_h query = nullptr;
	contacts_query_create(_contacts_person._uri, &query);
	contacts_query_set_filter(query, filter);

	contacts_list_h list = nullptr;
	contacts_db_get_records_with_query(query, 0, 0, &list);

	contacts_record_h record = nullptr;
	contacts_list_get_current_record_p(list, &record);

	contacts_filter_destroy(filter);
	contacts_query_destroy(query);
	contacts_list_destroy(list, false);
	return record;
}

contacts_list_h MembersProvider::getMembersList() const
{
	if (!m_GroupId) {
		return nullptr;
	}
	contacts_query_h query = getMembersQuery(m_GroupId);
	contacts_list_h list = nullptr;
	contacts_db_get_records_with_query(query, 0, 0, &list);

	contacts_query_destroy(query);
	return list;
}

void MembersProvider::onChanged(const char *uri)
{
	contacts_list_h changes = nullptr;
	contacts_db_get_changes_by_version(_contacts_grouprel_updated_info._uri, 0,
			m_DbVersion, &changes, &m_DbVersion);
	bool isChanged = false;

	for (auto &&record : makeRange(changes)) {
		if (getRecordInt(record, _contacts_grouprel_updated_info.group_id) != m_GroupId) {
			continue;
		}

		int contactId = getRecordInt(record, _contacts_grouprel_updated_info.contact_id);
		int changeType = getRecordInt(record, _contacts_grouprel_updated_info.type);
		switch (changeType) {
			case CONTACTS_CHANGE_INSERTED:
				insertPerson(getPersonRecord(contactId, ContactId));
				break;

			case CONTACTS_CHANGE_DELETED:
				auto personIt = findPerson(contactId, ContactId);
				if (personIt != getDataList().end()) {
					deletePerson(personIt);
				}
				break;
		}
		isChanged = true;
	}
	contacts_list_destroy(changes, true);

	if (isChanged) {
		onUpdateFinished();
	}
}

void MembersProvider::onGroupChanged(const char *uri)
{
	contacts_list_h changes = nullptr;
	contacts_db_get_changes_by_version(_contacts_group_updated_info._uri, 0,
			m_GroupDbVersion, &changes, &m_GroupDbVersion);
	bool isChanged = false;

	for (auto &&record : makeRange(changes)) {
		if (getRecordInt(record, _contacts_group_updated_info.group_id) != m_GroupId) {
			continue;
		}
		int changeType = getRecordInt(record, _contacts_group_updated_info.type);
		switch (changeType) {
			case CONTACTS_CHANGE_UPDATED:
				isChanged = true;
				break;
		}
	}
	contacts_list_destroy(changes, true);

	if (isChanged) {
		onUpdateFinished();
	}
}
