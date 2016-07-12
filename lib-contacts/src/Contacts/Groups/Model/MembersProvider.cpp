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

#include "Common/Database/RecordIterator.h"
#include "Common/Database/RecordUtils.h"

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
}

const int MembersProvider::getMembersCount() const
{
	if (!m_GroupId) {
		return 0;
	}

	contacts_query_h query = getFilteredQuery();
	int count = 0;
	contacts_db_get_count_with_query(query, &count);
	contacts_query_destroy(query);
	return count;
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
	contacts_query_h query = getFilteredQuery();
	contacts_list_h list = nullptr;
	contacts_db_get_records_with_query(query, 0, 0, &list);

	contacts_query_destroy(query);
	return list;
}

contacts_query_h MembersProvider::getFilteredQuery() const
{
	contacts_filter_h filter = nullptr;
	contacts_filter_create(_contacts_person_group_assigned._uri, &filter);
	contacts_filter_add_int(filter, _contacts_person_group_assigned.group_id,
			CONTACTS_MATCH_EQUAL, m_GroupId);

	contacts_query_h query = nullptr;
	contacts_query_create(_contacts_person_group_assigned._uri, &query);
	contacts_query_set_filter(query, filter);

	contacts_filter_destroy(filter);
	return query;
}
