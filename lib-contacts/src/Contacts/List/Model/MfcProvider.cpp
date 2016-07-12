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
#include "Common/Database/RecordIterator.h"
#include "Common/Database/RecordUtils.h"
#include "Contacts/List/Model/MfcProvider.h"
#include "Contacts/List/Model/Person.h"

#include "Utils/Callback.h"
#include "Utils/Logger.h"

using namespace Common::Database;
using namespace Contacts::List::Model;

#define MFC_MAX_COUNT 5

MfcProvider::MfcProvider()
{
	contacts_db_add_changed_cb(_contacts_phone_log._uri,
			makeCallbackWithLastParam(&MfcProvider::onPersonUsageUpdate), this);
}

MfcProvider::~MfcProvider()
{
	contacts_db_remove_changed_cb(_contacts_phone_log._uri,
			makeCallbackWithLastParam(&MfcProvider::onPersonUsageUpdate), this);
}

contacts_list_h MfcProvider::getPersonList() const
{
	contacts_list_h usageList = getPersonUsageList();
	contacts_list_h personList = nullptr;
	contacts_list_create(&personList);

	for (auto &&usageRecord : makeRange(usageList)) {
		int personId = getRecordInt(usageRecord, _contacts_person_usage.person_id);
		contacts_record_h personRecord = nullptr;
		contacts_db_get_record(_contacts_person._uri, personId, &personRecord);
		contacts_list_add(personList, personRecord);
	}

	contacts_list_destroy(usageList, true);
	return personList;
}

contacts_record_h MfcProvider::getPersonRecord(int id, PersonProvider::IdType idType) const
{
	if (idType == ContactId) {
		return nullptr;
	}

	for (auto &&usageRecord : makeRange(getPersonUsageList())) {
		if (getRecordInt(usageRecord, _contacts_person_usage.person_id) == id) {
			contacts_record_h record = nullptr;
			contacts_db_get_record(_contacts_person._uri, id, &record);
			return record;
		}
	}

	return nullptr;
}

void MfcProvider::insertPerson(contacts_record_h personRecord)
{
	if (!personRecord) {
		return;
	}

	update();
}

void MfcProvider::deletePerson(DataList::const_iterator personIt)
{
	reload();
}

contacts_list_h MfcProvider::getPersonUsageList() const
{
	contacts_list_h list = nullptr;
	contacts_query_h query = nullptr;

	contacts_query_create(_contacts_person_usage._uri, &query);
	contacts_query_set_sort(query, _contacts_person_usage.times_used, false);

	if (query) {
		contacts_filter_h filter = nullptr;
		contacts_filter_create(_contacts_person_usage._uri, &filter);

		if (filter) {
			contacts_filter_add_int(filter, _contacts_person_usage.times_used, CONTACTS_MATCH_GREATER_THAN, 0);
			contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND);
			contacts_filter_add_bool(filter, _contacts_person_usage.is_favorite, false);

			contacts_filter_h usageTypeFilter = nullptr;
			contacts_filter_create(_contacts_person_usage._uri, &usageTypeFilter);
			if (usageTypeFilter) {
				contacts_filter_add_int(usageTypeFilter, _contacts_person_usage.usage_type, CONTACTS_MATCH_EQUAL, CONTACTS_USAGE_STAT_TYPE_OUTGOING_CALL);
				contacts_filter_add_operator(usageTypeFilter, CONTACTS_FILTER_OPERATOR_OR);
				contacts_filter_add_int(usageTypeFilter, _contacts_person_usage.usage_type, CONTACTS_MATCH_EQUAL, CONTACTS_USAGE_STAT_TYPE_INCOMING_CALL);

				contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND);
				contacts_filter_add_filter(filter, usageTypeFilter);
				contacts_filter_destroy(usageTypeFilter);
			}

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
	update();
}

bool MfcProvider::update()
{
	contacts_list_h mfcList = getPersonList();
	int count = 0;
	contacts_list_get_count(mfcList, &count);

	const DataList &dataList = getDataList();

	if (dataList.size() == (size_t)count) {
		bool isEqual = std::equal(dataList.begin(), dataList.end(), begin(mfcList), equalPredicate);
		contacts_list_first(mfcList);

		if (!isEqual && count) {
			reload();
			return true;
		}

	} else {
		reload();
		return true;
	}

	contacts_list_destroy(mfcList, true);
	return false;
}

bool MfcProvider::equalPredicate(::Model::DataItem *data, contacts_record_h record)
{
	Person *person = (Person *)data;
	return person->getId() == getRecordInt(record, _contacts_person.id);
}

