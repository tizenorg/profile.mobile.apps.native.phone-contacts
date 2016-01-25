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
		m_ListFilterType(filterType), m_NewLogGroupCallback(nullptr)
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

LogGroupList LogProvider::getLogGroupList()
{
	if (m_GroupLogs.empty()) {
		if (m_Logs.empty()) {
			fillList(m_Logs);
		}
		fillGroupList(m_Logs, m_GroupLogs);
	}
	return m_GroupLogs;
}

void LogProvider::setNewLogCallback(NewLogGroupCallback callback)
{
	m_NewLogGroupCallback = std::move(callback);
}

void LogProvider::unsetNewLogCallback()
{
	m_NewLogGroupCallback = nullptr;
}

void LogProvider::fillList(LogList &logList)
{
	contacts_list_h list = nullptr;
	contacts_db_get_all_records(_contacts_phone_log._uri, 0, 0, &list);
	contacts_record_h record = nullptr;

	CONTACTS_LIST_FOREACH(list, record) {
		logList.push_back(new Log(record));
	}

	contacts_list_destroy(list, false);
}

void LogProvider::fillGroupList(LogList &logList, LogGroupList &logGroupList)
{
	logGroupList.push_back(new LogGroup((*logList.begin())));

	for (auto &&it : logList) {
		addLogGroup(logGroupList, it);
	}
}

bool LogProvider::shouldGroupLogs(Log *log, LogGroup *prevLogGroup)
{
	Log *prevLog = prevLogGroup->getLogList().back();
	return (log->getType() == prevLog->getType()
			&& strcmp(log->getNumber(), prevLog->getNumber()) == 0);

}

LogGroup *LogProvider::addLogGroup(LogGroupList &logGroupList, Log *log)
{
	LogGroup *lastLogGroup = logGroupList.back();

	if (shouldGroupLogs(log, lastLogGroup)) {
		lastLogGroup->addLog(log);
	} else {
		LogGroup *logGroup = new LogGroup(log);
		logGroupList.push_back(logGroup);
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
	LogGroupSet changedGroupLogs;

	fillList(newLogList);

	logIterator newIt = newLogList.begin();

	deleteRemovedLogs(newIt, changedGroupLogs, newLogList);
	addNewLogs(newIt, changedGroupLogs, newLogList);

	for(auto &&it : changedGroupLogs) {
		it->callLogChangeCallback();
	}
}

void LogProvider::deleteRemovedLogs(logIterator &newIt, LogGroupSet &changedGroupLogs, LogList &newLogList)
{
	logIterator oldIt = m_Logs.begin();

	while (newLogList.end() != newIt && m_Logs.end() != oldIt) {
		if ((*newIt)->getId() != (*oldIt)->getId()) {
			LogGroup *group = (*oldIt)->getLogGroup();
			changedGroupLogs.insert(group);
			group->removeLog(*oldIt);
			oldIt = m_Logs.erase(oldIt);
		} else {
			newIt++;
			oldIt++;
		}
	}

	while (oldIt != m_Logs.end()) {
		LogGroup *group = (*oldIt)->getLogGroup();
		changedGroupLogs.insert(group);
		group->removeLog(*oldIt);
		oldIt = m_Logs.erase(oldIt);
	}
}

void LogProvider::addNewLogs(logIterator &newIt, LogGroupSet &changedGroupLogs, LogList &newLogList)
{
	LogGroupList newLogGroups;

	if (newIt != newLogList.end()) {
		if (m_GroupLogs.empty()) {
			m_Logs.push_back(*newIt);
			LogGroup *group = new LogGroup(*newIt);
			m_GroupLogs.push_back(group);
			newLogGroups.push_back(group);
			newIt++;
		} else {
			LogGroup *group = m_GroupLogs.back();
			if (shouldGroupLogs(*newIt, group)) {
				m_Logs.push_back(*newIt);
				group->addLog(*newIt);
				changedGroupLogs.insert(std::move(group));
				newIt++;
			}
		}
	}

	while (newIt != newLogList.end()) {
		m_Logs.push_back(*newIt);
		newLogGroups.push_back(addLogGroup(m_GroupLogs, *newIt));
		newIt++;
	}

	if (m_NewLogGroupCallback) {
		m_NewLogGroupCallback(std::move(newLogGroups));
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
