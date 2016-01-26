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

#include "Phone/Utils.h"
#include "Utils/Logger.h"

#include <contacts.h>

std::string Phone::getSpeedDialNumber(int speedNumber)
{
	std::string number;
	contacts_filter_h filter = NULL;
	contacts_query_h query = NULL;
	contacts_list_h list = NULL;

	contacts_filter_create(_contacts_speeddial._uri, &filter);
	contacts_filter_add_int(filter, _contacts_speeddial.speeddial_number, CONTACTS_MATCH_EQUAL, speedNumber);

	contacts_query_create(_contacts_speeddial._uri, &query);
	contacts_query_set_filter(query, filter);

	int err = contacts_db_get_records_with_query(query, 0, 1, &list);
	WARN_IF_ERR(err, "contacts_db_get_records_with_query() failed.");
	if (list) {
		contacts_record_h record = NULL;
		contacts_list_get_current_record_p(list, &record);
		if (record) {
			char *str = NULL;
			contacts_record_get_str_p(record, _contacts_speeddial.number, &str);
			if (str) {
				number = str;
			}
		}

		contacts_list_destroy(list, true);
	}

	contacts_query_destroy(query);
	contacts_filter_destroy(filter);

	return number;
}

bool Phone::addSpeedDialNumber(int speedNumber, int numberId)
{
	contacts_filter_h filter = nullptr;
	contacts_filter_create(_contacts_speeddial._uri, &filter);
	contacts_filter_add_int(filter, _contacts_speeddial.number_id, CONTACTS_MATCH_EQUAL, numberId);

	contacts_query_h query = nullptr;
	contacts_query_create(_contacts_speeddial._uri, &query);
	contacts_query_set_filter(query, filter);

	contacts_list_h list = nullptr;
	int err = contacts_db_get_records_with_query(query, 0, 1, &list);
	contacts_query_destroy(query);
	contacts_filter_destroy(filter);
	RETVM_IF_ERR(err, false, "contacts_db_get_records_with_query() failed.");

	int count = 0;
	contacts_list_get_count(list, &count);
	contacts_list_destroy(list, true);
	if (count > 0) {
		return false;
	}

	contacts_record_h record = nullptr;
	contacts_record_create(_contacts_speeddial._uri, &record);
	contacts_record_set_int(record, _contacts_speeddial.speeddial_number, speedNumber);
	contacts_record_set_int(record, _contacts_speeddial.number_id, numberId);
	err = contacts_db_insert_record(record, nullptr);
	contacts_record_destroy(record, true);
	RETVM_IF_ERR(err, false, "contacts_db_insert_record() failed.");

	return true;
}

std::string Phone::getLastCallNumber()
{
	std::string number;
	contacts_list_h list = NULL;
	contacts_query_h query = NULL;
	contacts_filter_h filter = NULL;

	contacts_filter_create(_contacts_person_phone_log._uri, &filter);
	contacts_filter_add_int(filter, _contacts_person_phone_log.log_type,
			CONTACTS_MATCH_GREATER_THAN_OR_EQUAL, CONTACTS_PLOG_TYPE_VOICE_INCOMING);
	contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND);
	contacts_filter_add_int(filter, _contacts_person_phone_log.log_type,
			CONTACTS_MATCH_LESS_THAN_OR_EQUAL, CONTACTS_PLOG_TYPE_VIDEO_BLOCKED);

	contacts_query_create(_contacts_person_phone_log._uri, &query);
	contacts_query_set_filter(query, filter);
	contacts_query_set_sort(query, _contacts_person_phone_log.log_time, false);

	int err = contacts_db_get_records_with_query(query, 0, 1, &list);
	WARN_IF_ERR(err, "contacts_db_get_records_with_query() failed.");
	if (list) {
		contacts_record_h record = NULL;
		contacts_list_get_current_record_p(list, &record);
		if (record) {
			char *str = NULL;
			contacts_record_get_str_p(record, _contacts_person_phone_log.address, &str);
			if (str) {
				number = str;
			}
		}

		contacts_list_destroy(list, true);
	}

	contacts_query_destroy(query);
	contacts_filter_destroy(filter);

	return number;
}
