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
#include "Logs/Model/LogRecord.h"
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

LogList LogProvider::getLogList()
{
	LogList logList;

	fillList(logList);

	std::for_each(logList.begin(), logList.end(), [this](LogPtr log) {
		m_Logs.insert({log->getPersonId(), log});
	});

	return logList;
}

void LogProvider::setLogChangeCallback(LogChangeCallback callback)
{
	m_LogCallback = callback;
}

void LogProvider::unsetLogChangeCallback()
{
	m_LogCallback = nullptr;
}

void LogProvider::fillList(LogList &logList)
{
	contacts_list_h list = fetchLogList();
	contacts_record_h record = nullptr;

	addFirstLog(logList, record, list);

	contacts_list_next(list);

	CONTACTS_LIST_FOREACH(list, record) {
		addLog(logList, record);
	}

	contacts_list_destroy(list, false);
}

bool LogProvider::shouldGroupLogs(LogPtr log, LogPtr prevLog)
{
	return (log->getType() == prevLog->getType()
			&& strcmp(log->getNumber(), prevLog->getNumber()) == 0);
}

LogGroupPtr LogProvider::groupLogs(LogPtr log, LogPtr prevLog)
{
	LogGroupPtr logGroup = nullptr;
	if (prevLog->isGroup()) {
		logGroup = std::static_pointer_cast<LogGroup>(prevLog);
		logGroup->addLog(std::move(log));
	} else {
		logGroup = LogGroupPtr(new LogGroup());
		logGroup->addLog(std::move(prevLog));
		logGroup->addLog(std::move(log));
	}
	return logGroup;
}

void LogProvider::addLog(LogList &logList, contacts_record_h record)
{
	LogPtr log = LogPtr(new LogRecord(record));
	LogPtr lastLog = logList.back();

	if (shouldGroupLogs(log, lastLog)) {
		logList.pop_back();
		log = groupLogs(std::move(log), std::move(lastLog));
	}
	logList.push_back(log);
}

void LogProvider::addFirstLog(LogList &logList, contacts_record_h record, contacts_list_h list)
{
	if (contacts_list_get_current_record_p(list, &record) != CONTACTS_ERROR_NONE) {
		return;
	}

	LogPtr log = LogPtr(new LogRecord(record));
	logList.push_back(log);
}

contacts_filter_h LogProvider::getProviderFilter(LogProvider::FilterType filterType)
{
	contacts_filter_h filter = nullptr;
	contacts_filter_create(_contacts_phone_log._uri, &filter);

	if (filterType != LogProvider::FilterMissed) {
		contacts_filter_add_int(filter, _contacts_phone_log.log_type, CONTACTS_MATCH_GREATER_THAN_OR_EQUAL, CONTACTS_PLOG_TYPE_VOICE_INCOMMING_UNSEEN);
		contacts_filter_add_int(filter, _contacts_phone_log.log_type, CONTACTS_MATCH_LESS_THAN_OR_EQUAL, CONTACTS_PLOG_TYPE_VOICE_INCOMMING_SEEN);
	} else {
		contacts_filter_add_int(filter, _contacts_phone_log.log_type, CONTACTS_MATCH_GREATER_THAN_OR_EQUAL, CONTACTS_PLOG_TYPE_VOICE_INCOMMING);
		contacts_filter_add_int(filter, _contacts_phone_log.log_type, CONTACTS_MATCH_LESS_THAN_OR_EQUAL, CONTACTS_PLOG_TYPE_VIDEO_BLOCKED);
	}

	return filter;
}

contacts_list_h LogProvider::fetchLogList()
{
	contacts_list_h list = nullptr;
	contacts_query_h query = nullptr;
	contacts_filter_h filter = getProviderFilter(m_ListFilterType);

	contacts_filter_create(_contacts_phone_log._uri, &filter);

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
	m_LogCallback(getLogList());
}

void LogProvider::onContactChanged(const char *viewUri)
{
	contacts_list_h changes = nullptr;
	contacts_db_get_changes_by_version(_contacts_contact_updated_info._uri, 0, m_DbVersion, &changes, &m_DbVersion);

	contacts_record_h record = nullptr;
	CONTACTS_LIST_FOREACH(changes, record) {
		int contactId = 0;
		int changeType = -1;

		contacts_record_get_int(record, _contacts_contact_updated_info.contact_id, &contactId);
		contacts_record_get_int(record, _contacts_contact_updated_info.type, &changeType);

		notifyLogWithChange(contactId, static_cast<contacts_changed_e>(changeType));
	}
	contacts_list_destroy(changes, true);
}

void LogProvider::notifyLogWithChange(int contactId, contacts_changed_e changeType)
{
	switch (changeType) {
		case CONTACTS_CHANGE_INSERTED:
		{
			auto range = m_Logs.equal_range(contactId);
			for (auto it = range.first; it != range.second; ++it) {
				LogPtr log = it->second;
				if (log->getPersonId() != 0) {
					log->callContactChangeCallback(std::move(log), changeType);
				}
			}
		}
			break;
		case CONTACTS_CHANGE_UPDATED:
		case CONTACTS_CHANGE_DELETED:
		{
			auto range = m_Logs.equal_range(contactId);
			for (auto it = range.first; it != range.second; ++it) {
				LogPtr log = it->second;
				log->callContactChangeCallback(std::move(log), changeType);
			}
		}
			break;
	}
}
