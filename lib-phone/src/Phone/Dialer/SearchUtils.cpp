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

#include "Phone/Dialer/SearchUtils.h"
#include "Contacts/Utils.h"
#include "Ui/Thumbnail.h"
#include "Utils/Logger.h"

using namespace Phone::Dialer;
using namespace Phone::Dialer::Utils;

#define TAG_MATCH_PREFIX "<match>"
#define TAG_MATCH_SUFFIX "</match>"

namespace
{
	const char *mask[] = {
		"+", "", "abc", "def",
		"ghi", "jkl", "mno",
		"pqrs", "tuv", "wxyz"
	};

	char getDigit(char c)
	{
		static const char hash[] = "22233344455566677778889999";

		if (c == '+') {
			return '0';
		}

		size_t index = tolower(c) - 'a';
		if (index >= sizeof(hash) - 1) {
			return '\0';
		}

		return hash[index];
	}

	std::string getDigitMask(char digit)
	{
		if (digit > '9' || digit < '0') {
			ERR("getDigitMask expected digit(0-9) argument, but %c is provided", digit);
			return "";
		}

		int index = digit - '0';
		return mask[index];
	}

	contacts_filter_h createContactNameFilters(char digit)
	{
		std::string mask = getDigitMask(digit);
		if (mask.empty()) {
			return NULL;
		}

		contacts_filter_h filter = NULL;
		contacts_filter_create(_contacts_contact_number._uri, &filter);
		bool isFirst = true;
		for (auto &x : mask) {
			if (!isFirst) {
				contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_OR);
			}

			char str[] = { x, '\0' };
			contacts_filter_add_str(filter, _contacts_contact_number.display_name, CONTACTS_MATCH_CONTAINS, str);
			isFirst = false;
		}
		return filter;
	}

	contacts_list_h runQuery(const char *tableUri, contacts_filter_h filter)
	{
		contacts_list_h list = NULL;
		contacts_query_h query = NULL;

		contacts_query_create(tableUri, &query);
		contacts_query_set_filter(query, filter);

		contacts_db_get_records_with_query(query, 0, 0, &list);

		contacts_query_destroy(query);
		contacts_filter_destroy(filter);

		return list;
	}

	std::string getThumbnail(int id)
	{
		std::string path;
		contacts_record_h record = NULL;
		if (contacts_db_get_record(_contacts_contact._uri, id, &record) == CONTACTS_ERROR_NONE) {
			char *imgPath = NULL;
			contacts_record_get_str_p(record, _contacts_contact.image_thumbnail_path, &imgPath);
			if (imgPath) {
				path = imgPath;
			}
			contacts_record_destroy(record, true);
		}
		return path;
	}
}

contacts_list_h Phone::Dialer::Utils::getSpeedDial(char number)
{
	contacts_filter_h filter = NULL;
	contacts_filter_create(_contacts_speeddial._uri, &filter);
	contacts_filter_add_int(filter, _contacts_speeddial.speeddial_number, CONTACTS_MATCH_EQUAL, number - '0');

	contacts_list_h list = runQuery(_contacts_speeddial._uri, filter);

	return list;
}

contacts_list_h Phone::Dialer::Utils::getLogList(char digit)
{
	char number[] = { digit, '\0' };
	contacts_filter_h filter = NULL;
	contacts_filter_create(_contacts_phone_log._uri, &filter);

	contacts_filter_add_str(filter, _contacts_phone_log.address, CONTACTS_MATCH_CONTAINS, number);
	contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND);
	contacts_filter_add_int(filter, _contacts_phone_log.log_type,
			CONTACTS_MATCH_GREATER_THAN_OR_EQUAL, CONTACTS_PLOG_TYPE_VOICE_INCOMING);
	contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND);
	contacts_filter_add_int(filter, _contacts_phone_log.log_type,
			CONTACTS_MATCH_LESS_THAN_OR_EQUAL, CONTACTS_PLOG_TYPE_VIDEO_BLOCKED);

	contacts_list_h list = runQuery(_contacts_phone_log._uri, filter);

	contacts_list_h retList = NULL;
	contacts_list_create(&retList);
	contacts_record_h record = NULL;
	CONTACTS_LIST_FOREACH(list, record) {
		int personId = 0;
		contacts_record_get_int(record, _contacts_phone_log.person_id, &personId);
		if (personId == 0) {
			contacts_list_add(retList, record);
		} else {
			contacts_record_destroy(record, true);
		}
	}
	contacts_list_destroy(list, false);

	return retList;
}

contacts_list_h Phone::Dialer::Utils::getContactListByName(char digit)
{
	contacts_query_h query = NULL;
	contacts_list_h list = NULL;
	contacts_query_create(_contacts_contact_number._uri, &query);
	contacts_filter_h filter = createContactNameFilters(digit);
	if (filter) {
		contacts_query_set_filter(query, filter);
		contacts_db_get_records_with_query(query, 0, 0, &list);
	}

	contacts_filter_destroy(filter);
	contacts_query_destroy(query);
	return list;
}

contacts_list_h Phone::Dialer::Utils::getContactListByNumber(char digit)
{
	char number[] = { digit, '\0' };
	contacts_filter_h filter = NULL;
	contacts_filter_create(_contacts_contact_number._uri, &filter);
	contacts_filter_add_str(filter, _contacts_contact_number.number, CONTACTS_MATCH_CONTAINS, number);

	contacts_list_h list = runQuery(_contacts_contact_number._uri, filter);

	return list;
}

std::string Phone::Dialer::Utils::contactNameToMask(const std::string &name)
{
	std::string number;
	number.reserve(name.size());

	for (auto &x : name) {
		char digit = getDigit(x);
		if (digit == '\0') {
			break;
		}
		number.push_back(digit);
	}
	return number;
}

std::string Phone::Dialer::Utils::highlightTextByPos(std::string &text, size_t position, size_t length)
{
	std::string highlightText = text;
	size_t endPos = position + length + sizeof(TAG_MATCH_PREFIX) - 1;

	highlightText.insert(position, TAG_MATCH_PREFIX);
	highlightText.insert(endPos, TAG_MATCH_SUFFIX);

	return highlightText;
}

Evas_Object *Phone::Dialer::Utils::createThumbnail(Evas_Object *parent, int contactId)
{
	using Ui::Thumbnail;

	Thumbnail *thumbnail = Thumbnail::create(parent, Thumbnail::SizeSmall,
			getThumbnail(contactId).c_str());
	thumbnail->setSizeHint(true);
	return thumbnail->getEvasObject();
}
