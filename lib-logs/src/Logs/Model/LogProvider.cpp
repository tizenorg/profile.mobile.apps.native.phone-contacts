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

#include "Logs/Model/LogProvider.h"
#include "Logs/Model/LogGroup.h"
#include "Contacts/Utils.h"
#include "Utils/Logger.h"
#include "Utils/Callback.h"

#include <algorithm>

using namespace Logs::Model;

LogProvider::LogProvider(LogProvider::FilterType filterType):
		m_ListFilterType(filterType)
{
	contacts_db_get_current_version(&m_DbVersion);
	contacts_db_add_changed_cb(_contacts_phone_log._uri, makeCallbackWithLastParam(&LogProvider::onLogChanged), this);
	contacts_db_add_changed_cb(_contacts_person._uri, makeCallbackWithLastParam(&LogProvider::onContactChanged), this);
}

LogProvider::~LogProvider()
{
	contacts_db_remove_changed_cb(_contacts_phone_log._uri, makeCallbackWithLastParam(&LogProvider::onLogChanged), this);
	contacts_db_remove_changed_cb(_contacts_person._uri, makeCallbackWithLastParam(&LogProvider::onContactChanged), this);
}

LogGroupList LogProvider::getLogList()
{
	if (m_Logs.empty()) {
		fillList(m_Logs);
	}
	return m_Logs;
}

void LogProvider::setNewLogCallback(NewLogGroupCallback callback)
{
	m_NewLogGroupCallback = std::move(callback);
}

void LogProvider::unsetNewLogCallback()
{
	m_NewLogGroupCallback = nullptr;
}

void LogProvider::fillList(LogGroupList &logList)
{
	contacts_list_h list = fetchLogList();

	logList.push_back(getFirstLogGroup(list));

	contacts_list_next(list);

	contacts_record_h record = nullptr;
	CONTACTS_LIST_FOREACH(list, record) {
		addLogGroup(logList, record);
	}

	contacts_list_destroy(list, false);
}

bool LogProvider::shouldGroupLogs(LogPtr log, LogGroupPtr prevLogGroup)
{
	LogPtr prevLog = prevLogGroup->getLogList().back();
	return (log->getType() == prevLog->getType()
			&& strcmp(log->getNumber(), prevLog->getNumber()) == 0);
}

void LogProvider::addLogGroup(LogGroupList &logList, contacts_record_h record)
{
	LogPtr log = LogPtr(new Log(record));
	LogGroupPtr lastLogGroup = logList.back();

	if (shouldGroupLogs(log, lastLogGroup)) {
		lastLogGroup->addLog(std::move(log));
	} else {
		LogGroupPtr logGroup = LogGroupPtr(new LogGroup(std::move(log)));
		logList.push_back(std::move(logGroup));
	}
}

LogGroupPtr LogProvider::getFirstLogGroup(contacts_list_h list)
{
	contacts_record_h record = nullptr;
	if (contacts_list_get_current_record_p(list, &record) != CONTACTS_ERROR_NONE) {
		return nullptr;
	}

	return LogGroupPtr(new LogGroup(LogPtr(new Log(record))));
}

contacts_filter_h LogProvider::getFilter(LogProvider::FilterType filterType)
{
	contacts_filter_h filter = nullptr;
	contacts_filter_create(_contacts_phone_log._uri, &filter);

	if (filterType != LogProvider::FilterMissed) {
		contacts_filter_add_int(filter, _contacts_phone_log.log_type, CONTACTS_MATCH_GREATER_THAN_OR_EQUAL, CONTACTS_PLOG_TYPE_VOICE_INCOMMING_UNSEEN);
		contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND);
		contacts_filter_add_int(filter, _contacts_phone_log.log_type, CONTACTS_MATCH_LESS_THAN_OR_EQUAL, CONTACTS_PLOG_TYPE_VOICE_INCOMMING_SEEN);
	} else {
		contacts_filter_add_int(filter, _contacts_phone_log.log_type, CONTACTS_MATCH_GREATER_THAN_OR_EQUAL, CONTACTS_PLOG_TYPE_VOICE_INCOMMING);
		contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND);
		contacts_filter_add_int(filter, _contacts_phone_log.log_type, CONTACTS_MATCH_LESS_THAN_OR_EQUAL, CONTACTS_PLOG_TYPE_VIDEO_BLOCKED);
	}

	return filter;
}

contacts_list_h LogProvider::fetchLogList()
{
	contacts_list_h list = nullptr;
	contacts_query_h query = nullptr;
	contacts_filter_h filter = getFilter(m_ListFilterType);

	contacts_query_create(_contacts_phone_log._uri, &query);
	contacts_query_set_filter(query, filter);
	contacts_query_set_sort(query, _contacts_phone_log.log_time, true);
	contacts_db_get_records_with_query(query, 0, 0, &list);

	contacts_filter_destroy(filter);
	contacts_query_destroy(query);

	return list;
}

void LogProvider::onLogChanged(const char *viewUri)
{
	LogGroupList changedLogList;
	LogGroupList newLogList;

	fillList(changedLogList);

	auto changedIt = changedLogList.rbegin();
	auto oldIt = m_Logs.rbegin();

	while (changedLogList.rend() != changedIt || m_Logs.rend() != oldIt) {
		int listCount = (*oldIt)->getLogList().size();
		int removeCount = (*oldIt)->removeDeletedLogs(*changedIt);
		int addCount = (*oldIt)->addNewLogs(*changedIt);

		if (listCount == removeCount && addCount == 0) {
			(*oldIt)->callLogRemoveCallback();
			m_Logs.erase(oldIt.base());
		} else if (addCount != 0 || (removeCount != 0 && removeCount < listCount)) {
			(*oldIt)->callLogChangeCallback();
		} else {
			++changedIt;
		}
		++oldIt;
	}

	auto it = changedLogList.begin();
	int oldFirstId = (m_Logs.back())->getFirstLog()->getId();

	while ((*it)->getFirstLog()->getId() != oldFirstId) {
		m_Logs.push_back(*it);
		m_NewLogGroupCallback(std::move(*it));
		++it;
	}
}

void LogProvider::onContactChanged(const char *viewUri)
{
	contacts_list_h changes = nullptr;
	contacts_db_get_changes_by_version(_contacts_contact_updated_info._uri, 0, m_DbVersion, &changes, &m_DbVersion);

	contacts_record_h record = nullptr;
	CONTACTS_LIST_FOREACH(changes, record) {
		notifyLogWithChange(record);
	}
	contacts_list_destroy(changes, true);
}

void LogProvider::notifyLogWithChange(contacts_record_h record)
{
	contacts_record_h contactRecord = nullptr;
	int personId = 0;
	int contactId = 0;

	contacts_record_get_int(record, _contacts_contact_updated_info.contact_id, &contactId);
	contacts_db_get_record(_contacts_contact._uri, contactId, &contactRecord);
	contacts_record_get_int(contactRecord, _contacts_contact.person_id, &personId);

	for (auto &&it : m_Logs) {
		it->updateGroup();

		if (it->changedPersonId(personId)) {
			it->callLogChangeCallback();
		} else {
			int changeType = -1;
			contacts_record_get_int(record, _contacts_contact_updated_info.type, &changeType);
			LogPtr log = it->getFirstLog();
			if (CONTACTS_CHANGE_UPDATED == changeType && personId == log->getPersonId()) {

				bool imageChanged = false;
				contacts_record_get_bool(record, _contacts_contact_updated_info.image_changed, &imageChanged);
				if (imageChanged == true || it->changedName()) {
					it->callLogChangeCallback();
				}
			}
		}
	}

	contacts_record_destroy(contactRecord, true);
}
