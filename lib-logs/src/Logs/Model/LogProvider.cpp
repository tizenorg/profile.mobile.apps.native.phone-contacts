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
#include "Contacts/Utils.h"
#include "Utils/Logger.h"
#include "Utils/Callback.h"

#include <algorithm>

using namespace Logs::Model;

LogProvider::LogProvider(FilterType filterType):
		m_FilterType(filterType), m_InsertCallback(nullptr)
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

const LogProvider::LogGroupList &LogProvider::getLogGroupList()
{
	if (m_Groups.empty()) {
		if (m_Logs.empty()) {
			fillList(m_Logs);
		}
		fillGroupList(m_Logs, m_Groups);
	}
	return m_Groups;
}

void LogProvider::setInsertCallback(InsertCallback callback)
{
	m_InsertCallback = std::move(callback);
}

void LogProvider::unsetInsertCallback()
{
	m_InsertCallback = nullptr;
}

void LogProvider::fillList(LogList &logList)
{
	contacts_list_h list = fetchLogList();
	contacts_db_get_all_records(_contacts_phone_log._uri, 0, 0, &list);
	contacts_record_h record = nullptr;

	CONTACTS_LIST_FOREACH(list, record) {
		logList.push_back(LogPtr(new Log(record)));
	}

	contacts_list_destroy(list, false);
}

void LogProvider::fillGroupList(LogList &logList, LogGroupList &logGroupList)
{
	if (logList.empty()) {
		return;
	}
	LogList::reverse_iterator log = logList.rbegin();
	logGroupList.push_back(LogGroupPtr(new LogGroup((*log).get())));

	for (++log; log != logList.rend(); log++) {
		addLog(logGroupList, (*log).get());
	}
}

bool LogProvider::shouldGroupLogs(Log *log, LogGroup *prevLogGroup)
{
	Log *prevLog = prevLogGroup->getLogList().back();
	return (log->getType() == prevLog->getType()
			&& strcmp(log->getNumber(), prevLog->getNumber()) == 0);
}

LogGroup *LogProvider::addLog(LogGroupList &logGroupList, Log *log)
{
	LogGroup *lastLogGroup = logGroupList.back().get();

	if (shouldGroupLogs(log, lastLogGroup)) {
		lastLogGroup->addLog(log);
	} else {
		LogGroup *logGroup = new LogGroup(log);
		logGroupList.push_back(LogGroupPtr(logGroup));
		return logGroup;
	}
	return nullptr;
}

contacts_filter_h LogProvider::getFilter(LogProvider::FilterType filterType)
{
	contacts_filter_h filter = nullptr;
	contacts_filter_create(_contacts_phone_log._uri, &filter);

	if (filterType != LogProvider::FilterMissed) {
		contacts_filter_add_int(filter, _contacts_phone_log.log_type, CONTACTS_MATCH_GREATER_THAN_OR_EQUAL, CONTACTS_PLOG_TYPE_VOICE_INCOMING_UNSEEN);
		contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND);
		contacts_filter_add_int(filter, _contacts_phone_log.log_type, CONTACTS_MATCH_LESS_THAN_OR_EQUAL, CONTACTS_PLOG_TYPE_VOICE_INCOMING_SEEN);
	} else {
		contacts_filter_add_int(filter, _contacts_phone_log.log_type, CONTACTS_MATCH_GREATER_THAN_OR_EQUAL, CONTACTS_PLOG_TYPE_VOICE_INCOMING);
		contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND);
		contacts_filter_add_int(filter, _contacts_phone_log.log_type, CONTACTS_MATCH_LESS_THAN_OR_EQUAL, CONTACTS_PLOG_TYPE_VIDEO_BLOCKED);
	}

	return filter;
}

contacts_list_h LogProvider::fetchLogList()
{
	contacts_list_h list = nullptr;
	contacts_query_h query = nullptr;
	contacts_filter_h filter = getFilter(m_FilterType);

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
	LogList newLogList;

	fillList(newLogList);

	LogIterator newIt = newLogList.begin();

	deleteRemovedLogs(newIt, newLogList);
	addNewLogs(newIt, newLogList);
}

void LogProvider::onGroupChanged(LogGroup *group)
{
	if (group->getLogList().empty()) {
		group->onChange(LogGroup::Remove);
		m_Groups.remove_if([group](const LogGroupPtr &groupPtr) {
			return groupPtr.get() == group;
		});
	} else {
		group->onChange(LogGroup::Update);
	}
}

void LogProvider::onGroupInsert(LogGroup *group)
{
	if (m_InsertCallback) {
		m_InsertCallback(group);
	}
}

void LogProvider::deleteRemovedLogs(LogIterator &newIt, LogList &newLogList)
{
	LogIterator oldIt = m_Logs.begin();
	LogGroup *prevGroup = nullptr;

	bool isChanged = false;
	while (newIt != newLogList.end() && oldIt != m_Logs.end()) {
		if (prevGroup != (*oldIt)->getLogGroup()) {
			if (isChanged == true) {
				onGroupChanged(prevGroup);
				isChanged = false;
			}
			prevGroup = (*oldIt)->getLogGroup();
		}

		if ((*newIt)->getId() != (*oldIt)->getId()) {
			isChanged = true;
			oldIt = m_Logs.erase(oldIt);
		} else {
			++newIt;
			++oldIt;
		}
	}

	while (oldIt != m_Logs.end()) {
		if (prevGroup != (*oldIt)->getLogGroup()) {
			if (isChanged) {
				onGroupChanged(prevGroup);
				isChanged = false;
			}
			prevGroup = (*oldIt)->getLogGroup();
		}

		isChanged = true;
		oldIt = m_Logs.erase(oldIt);
	}

	if (isChanged) {
		onGroupChanged(prevGroup);
	}
}

void LogProvider::addNewLogs(LogIterator &newIt, LogList &newLogList)
{
	LogGroup *group = nullptr;

	if (newIt != newLogList.end() && m_Groups.empty()) {
		group = new LogGroup(newIt->get());
		m_Logs.push_back(std::move(*newIt));
		m_Groups.push_back(LogGroupPtr(group));
		++newIt;
	} else {
		group = m_Groups.back().get();
		bool isChanged = false;

		LogGroup *newGroup = nullptr;
		for (; newIt != newLogList.end(); ++newIt) {
			newGroup = addLog(m_Groups, newIt->get());
			if (newGroup) {
				break;
			}
			isChanged = true;
		}
		if (isChanged) {
			group->onChange(LogGroup::Update);
		}

		group = newGroup;
	}

	for (; newIt != newLogList.end(); ++newIt) {
		LogGroup *newGroup = addLog(m_Groups, newIt->get());
		if (newGroup) {
			if (group) {
				onGroupInsert(group);
			}
			group = newGroup;
		}
		m_Logs.push_back(std::move(*newIt));
	}

	if (group) {
		onGroupInsert(group);
	}
}

void LogProvider::onContactChanged(const char *viewUri)
{
	contacts_list_h changes = nullptr;
	contacts_db_get_changes_by_version(_contacts_contact_updated_info._uri, 0, m_DbVersion, &changes, &m_DbVersion);

	contacts_record_h record = nullptr;
	CONTACTS_LIST_FOREACH(changes, record) {
		/*
		 * TODO
		 */
	}
	contacts_list_destroy(changes, true);
}
