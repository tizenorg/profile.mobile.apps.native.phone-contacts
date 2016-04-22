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

#include "Common/Database/RecordIterator.h"
#include "Common/Database/RecordUtils.h"
#include "Contacts/List/Model/MfcProvider.h"
#include "Contacts/List/Model/Person.h"

#include "Utils/Callback.h"
#include "Utils/Logger.h"

using namespace Common::Database;
using namespace Contacts;
using namespace Contacts::List::Model;
using namespace Contacts::Model;

#define MFC_MAX_COUNT 5

MfcProvider::MfcProvider()
{
	setUpdateMask(ChangeUpdate);

	contacts_db_add_changed_cb(_contacts_phone_log._uri,
			makeCallbackWithLastParam(&MfcProvider::onPersonUsageUpdate), this);
}

MfcProvider::~MfcProvider()
{
	contacts_db_remove_changed_cb(_contacts_phone_log._uri,
			makeCallbackWithLastParam(&MfcProvider::onPersonUsageUpdate), this);
}

contacts_filter_h MfcProvider::getFilter() const
{
	contacts_filter_h filter = nullptr;
	contacts_filter_create(_contacts_person_usage._uri, &filter);

	contacts_filter_add_int(filter, _contacts_person_usage.times_used, CONTACTS_MATCH_GREATER_THAN, 0);
	contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND);
	contacts_filter_add_int(filter, _contacts_person_usage.usage_type, CONTACTS_MATCH_EQUAL, CONTACTS_USAGE_STAT_TYPE_OUTGOING_CALL);
	contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND);
	contacts_filter_add_bool(filter, _contacts_person_usage.is_favorite, false);

	return filter;
}

contacts_query_h MfcProvider::getQuery() const
{
	contacts_query_h query = nullptr;

	contacts_query_create(_contacts_person_usage._uri, &query);
	contacts_query_set_sort(query, _contacts_person_usage.times_used, false);

	return query;
}

contacts_list_h MfcProvider::getPersonList() const
{
	contacts_list_h list = nullptr;
	contacts_query_h query = getQuery();
	if (query) {
		contacts_filter_h filter = getFilter();
		if (filter) {
			contacts_query_set_filter(query, filter);
		}

		int err = contacts_db_get_records_with_query(query, 0, MFC_MAX_COUNT, &list);
		WARN_IF_ERR(err, "contacts_db_get_records_with_query() failed.");

		contacts_query_destroy(query);
		contacts_filter_destroy(filter);
	}
	return list;
}

void MfcProvider::onPersonUsageUpdate(const char *viewUri)
{
	contacts_list_h mfcList = getPersonList();

	int mfcCount = 0;
	contacts_list_get_count(mfcList, &mfcCount);

	if (getDataList().size() == (size_t)mfcCount) {
		RecordIterator mfcIterator = RecordIterator(mfcList);

		for (auto &&data : getDataList()) {
			Person *person = static_cast<Person *>(data);

			if (getRecordInt(*mfcIterator, _contacts_person_usage.person_id)
					== person->getId()) {
				++mfcIterator;

			} else {
				updateMfcList();
				break;
			}
		}
	} else {
		updateMfcList();
	}

	contacts_list_destroy(mfcList, true);
}

void MfcProvider::updateMfcList()
{
	clearDataList();

	for (auto &&data : getDataList()) {
		Person *person = static_cast<Person *>(data);
		onInserted(*person);
	}
}
