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

#include "Common/Database/Queries.h"
#include "Utils/Logger.h"

using namespace Common;

contacts_list_h Database::getPersonContacts(int personId, Utils::Range<unsigned *> projection)
{
	contacts_filter_h filter = nullptr;
	contacts_filter_create(_contacts_contact._uri, &filter);
	contacts_filter_add_int(filter, _contacts_contact.person_id, CONTACTS_MATCH_EQUAL, personId);

	contacts_query_h query = nullptr;
	contacts_query_create(_contacts_contact._uri, &query);
	contacts_query_set_filter(query, filter);
	if (projection) {
		contacts_query_set_projection(query, projection.begin(), projection.count());
	}

	contacts_list_h list = nullptr;
	int err = contacts_db_get_records_with_query(query, 0, 0, &list);
	RETVM_IF_ERR(err, nullptr, "contacts_db_get_records_with_query failed.");

	return list;
}

int Database::getDisplayContactId(int personId)
{
	contacts_record_h record = 0;
	int err = contacts_db_get_record(_contacts_person._uri, personId, &record);
	RETVM_IF_ERR(err, 0, "contacts_db_get_record() failed.");

	int id = 0;
	contacts_record_get_int(record, _contacts_person.display_contact_id, &id);
	contacts_record_destroy(record, true);

	return id;
}

int Database::getPersonId(int contactId)
{
	contacts_record_h record = nullptr;
	int err = contacts_db_get_record(_contacts_contact._uri, contactId, &record);
	RETVM_IF_ERR(err, 0, "contacts_db_get_record() failed.");

	int id = 0;
	contacts_record_get_int(record, _contacts_contact.person_id, &id);
	contacts_record_destroy(record, true);

	return id;
}

contacts_list_h Database::getRecordList(const char *uri, unsigned propertyId, int value, contacts_filter_h parentFilter)
{
	contacts_filter_h filter = nullptr;
	contacts_filter_create(uri, &filter);
	contacts_filter_add_int(filter, propertyId, CONTACTS_MATCH_EQUAL, value);
	if (parentFilter) {
		contacts_filter_add_filter(parentFilter, filter);
	}

	contacts_query_h query = nullptr;
	contacts_query_create(uri, &query);
	contacts_query_set_filter(query, filter);

	contacts_list_h list = nullptr;
	int err = contacts_db_get_records_with_query(query, 0, 0, &list);
	RETVM_IF_ERR(err, nullptr, "contacts_db_get_records_with_query failed.");

	contacts_query_destroy(query);
	contacts_filter_destroy(filter);

	return list;
}

contacts_list_h Database::getRecordList(const char *uri, unsigned propertyId, Utils::Range<int *> values, contacts_filter_h parentFilter)
{
	contacts_filter_h filter = nullptr;
	contacts_filter_create(uri, &filter);
	for (auto it = values.begin(); it != values.end(); ++it) {
		if (it != values.begin()) {
			contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_OR);
		}

		contacts_filter_add_int(filter, propertyId, CONTACTS_MATCH_EQUAL, *it);
	}
	if (parentFilter) {
		contacts_filter_add_filter(parentFilter, filter);
	}

	contacts_query_h query = nullptr;
	contacts_query_create(uri, &query);
	contacts_query_set_filter(query, filter);

	contacts_list_h list = nullptr;
	int err = contacts_db_get_records_with_query(query, 0, 0, &list);
	RETVM_IF_ERR(err, nullptr, "contacts_db_get_records_with_query failed.");

	contacts_query_destroy(query);
	contacts_filter_destroy(filter);

	return list;
}
