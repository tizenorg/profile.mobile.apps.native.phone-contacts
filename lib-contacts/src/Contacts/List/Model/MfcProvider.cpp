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

#include "Contacts/List/Model/MfcProvider.h"
#include "Contacts/List/Model/Person.h"
#include "Contacts/Utils.h"
#include "Utils/Logger.h"

using namespace Contacts;
using namespace Contacts::List::Model;
using namespace Contacts::Model;

#define MFC_MAX_COUNT 5

namespace
{
	contacts_list_h getDbMfcList()
	{
		contacts_query_h query = nullptr;
		int err = contacts_query_create(_contacts_person_usage._uri, &query);
		RETVM_IF_ERR(err, nullptr, "contacts_query_create() failed.");

		contacts_filter_h filter = nullptr;
		contacts_filter_create(_contacts_person_usage._uri, &filter);

		contacts_filter_add_int(filter, _contacts_person_usage.times_used, CONTACTS_MATCH_GREATER_THAN, 0);
		contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND);
		contacts_filter_add_int(filter, _contacts_person_usage.usage_type, CONTACTS_MATCH_EQUAL, CONTACTS_USAGE_STAT_TYPE_OUTGOING_CALL);
		contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND);
		contacts_filter_add_bool(filter, _contacts_person_usage.is_favorite, false);

		contacts_query_set_filter(query, filter);

		contacts_query_set_sort(query, _contacts_person_usage.times_used, false);

		contacts_list_h list = nullptr;
		contacts_db_get_records_with_query(query, 0, MFC_MAX_COUNT, &list);

		contacts_query_destroy(query);
		contacts_filter_destroy(filter);

		return list;
	}
}

MfcProvider::~MfcProvider()
{
	clearMfcList();
}

const ContactDataList &MfcProvider::getContactDataList()
{
	contacts_list_h mfcList = ::getDbMfcList();
	if (m_MfcList.empty()) {
		createMfcList(mfcList);
	}
	contacts_list_destroy(mfcList, false);

	return m_MfcList;
}

bool MfcProvider::updateContactDataList()
{
	bool shouldUpdate = false;

	int mfcCount = 0;
	contacts_list_h mfcList = ::getDbMfcList();
	contacts_list_get_count(mfcList, &mfcCount);

	if (m_MfcList.size() == (size_t)mfcCount) {
		for (auto &&data : m_MfcList) {
			Person *person = static_cast<Person *>(data);

			contacts_record_h mfcRecord = nullptr;
			contacts_list_get_current_record_p(mfcList, &mfcRecord);

			int personId = 0;
			contacts_record_get_int(mfcRecord, _contacts_person_usage.person_id, &personId);

			if (person->getId() == personId) {
				contacts_list_next(mfcList);
			} else {
				shouldUpdate = true;
				break;
			}
		}
	} else {
		shouldUpdate = true;
	}

	if (shouldUpdate) {
		createMfcList(mfcList);
	}
	contacts_list_destroy(mfcList, false);

	return shouldUpdate;
}

void MfcProvider::createMfcList(contacts_list_h list)
{
	clearMfcList();

	contacts_record_h mfcRecord = nullptr;
	CONTACTS_LIST_FOREACH(list, mfcRecord) {
		int personId = 0;
		contacts_record_get_int(mfcRecord, _contacts_person_usage.person_id, &personId);

		contacts_record_h personRecord = nullptr;
		contacts_db_get_record(_contacts_person._uri, personId, &personRecord);

		m_MfcList.push_back(new Person(personRecord));
	}
}

void MfcProvider::clearMfcList()
{
	for (auto &&person : m_MfcList) {
		delete person;
	}
	m_MfcList.clear();
}
