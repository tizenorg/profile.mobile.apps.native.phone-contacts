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
		m_ListFilterType(filterType), m_InsertCallback(nullptr)
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

const GroupList &LogProvider::getLogGroupList()
{
	if (m_Groups.empty()) {
		if (m_Logs.empty()) {
			fillList(m_Logs);
		}
		fillGroupList(m_Logs, m_Groups);
	}
	return m_Groups;
}

void LogProvider::setNewLogCallback(InsertCallback callback)
{
	m_InsertCallback = std::move(callback);
}

void LogProvider::unsetNewLogCallback()
{
	m_InsertCallback = nullptr;
}

void LogProvider::fillList(LogList &logList)
{
	contacts_list_h list = nullptr;
	contacts_db_get_all_records(_contacts_phone_log._uri, 0, 0, &list);
	contacts_record_h record = nullptr;

	CONTACTS_LIST_FOREACH(list, record) {
		logList.push_back(UniqueLog(new Log(record)));
	}

	contacts_list_destroy(list, false);
}

void LogProvider::fillGroupList(LogList &logList, GroupList &logGroupList)
{
	logGroupList.push_back(UniqueGroup(new LogGroup((*logList.begin()).get())));

	for (auto &&log : logList) {
		addLog(logGroupList, log.get());
	}
}

bool LogProvider::shouldGroupLogs(Log *log, LogGroup *prevLogGroup)
{
	Log *prevLog = prevLogGroup->getLogList().back();
	return (log->getType() == prevLog->getType()
			&& strcmp(log->getNumber(), prevLog->getNumber()) == 0);
}

LogGroup *LogProvider::addLog(GroupList &logGroupList, Log *log)
{
	LogGroup *lastLogGroup = logGroupList.back().get();

	if (shouldGroupLogs(log, lastLogGroup)) {
		lastLogGroup->addLog(log);
	} else {
		LogGroup *logGroup = new LogGroup(log);
		m_Groups.push_back(UniqueGroup(logGroup));
		return logGroup;
	}
	return nullptr;
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
	LogList newLogList;

	fillList(newLogList);

	LogIterator newIt = newLogList.begin();

	deleteRemovedLogs(newIt, newLogList);
	addNewLogs(newIt, newLogList);
}

void LogProvider::onGroupChanged(LogGroup *prevGroup)
{
	prevGroup->onChange();
	if (prevGroup->getLogList().empty()) {
		m_Groups.remove_if(
			[prevGroup](const UniqueGroup &group) {
			return group.get() == prevGroup;
		});
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

	if (oldIt == m_Logs.end() && isChanged && newIt != newLogList.end()) {
		onGroupChanged(prevGroup);
		return;
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

	if (oldIt == m_Logs.end() && isChanged) {
		onGroupChanged(prevGroup);
		return;
	}
}

void LogProvider::addNewLogs(LogIterator &newIt, LogList &newLogList)
{
	std::list<LogGroup *> newLogGroups;

	if (newIt != newLogList.end() && m_Groups.empty()) {
		m_Logs.push_back(std::move(*newIt));
		LogGroup *group = new LogGroup((*newIt).get());
		m_Groups.push_back(UniqueGroup(group));
		m_InsertCallback(group);
		++newIt;
	}

	LogGroup *group = m_Groups.back().get();
	bool isChanged = false;

	while (newIt != newLogList.end()) {
		if (shouldGroupLogs((*newIt).get(), group)) {
			isChanged = true;
			group->addLog((*newIt).get());
			m_Logs.push_back(std::move(*newIt));
			++newIt;
		} else {
			if (isChanged) {
				isChanged = false;
				group->onChange();
			}

			LogGroup *group = addLog(m_Groups, (*newIt).get());
			if (group) {
				newLogGroups.push_back(std::move(group));
			}
			m_Logs.push_back(std::move(*newIt));
			++newIt;
		}
	}

	if (newIt != newLogList.end()) {
		if (isChanged) {
			isChanged = false;
			group->onChange();
		}
	}

	if (m_InsertCallback) {
		for (auto it = newLogGroups.begin(); newLogGroups.end() != it; ++it) {
			m_InsertCallback((*it));
		}
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
