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

#include "Contacts/Groups/Model/Queries.h"

using namespace Contacts::Groups;

contacts_query_h Model::getMembersQuery(int groupId)
{
	contacts_filter_h filter = nullptr;
	contacts_filter_create(_contacts_person_group_assigned._uri, &filter);
	contacts_filter_add_int(filter, _contacts_person_group_assigned.group_id,
			CONTACTS_MATCH_EQUAL, groupId);

	contacts_query_h query = nullptr;
	contacts_query_create(_contacts_person_group_assigned._uri, &query);
	contacts_query_set_filter(query, filter);

	contacts_filter_destroy(filter);
	return query;
}

int Model::getMembersCount(int groupId)
{
	if (!groupId) {
		return 0;
	}

	contacts_query_h query = getMembersQuery(groupId);
	int count = 0;
	contacts_db_get_count_with_query(query, &count);
	contacts_query_destroy(query);
	return count;
}
