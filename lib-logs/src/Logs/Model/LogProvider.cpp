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
#include "Utils/Range.h"

#include <algorithm>

using namespace Logs::Model;

LogProvider::LogProvider()
{
	contacts_db_get_current_version(&m_DbVersion);
	contacts_db_add_changed_cb(_contacts_phone_log._uri, makeCallbackWithLastParam(&LogProvider::onLogsChanged), this);
	contacts_db_add_changed_cb(_contacts_person._uri, makeCallbackWithLastParam(&LogProvider::onContactChanged), this);
}

LogProvider::~LogProvider()
{
	contacts_db_remove_changed_cb(_contacts_phone_log._uri, makeCallbackWithLastParam(&LogProvider::onLogsChanged), this);
	contacts_db_remove_changed_cb(_contacts_person._uri, makeCallbackWithLastParam(&LogProvider::onContactChanged), this);
}

const LogProvider::LogGroupList &LogProvider::getLogGroupList()
{
	if (m_Groups.empty()) {
		if (m_Logs.empty()) {
			fillList();
		}
		fillGroupList(m_Logs.begin(), m_Logs.end());
	}
	return m_Groups;
}

void LogProvider::resetLogGroups()
{
	m_Groups.clear();
	fillGroupList(m_Logs.begin(), m_Logs.end());
}

void LogProvider::setInsertCallback(InsertCallback callback)
{
	m_InsertCallback = std::move(callback);
}

void LogProvider::unsetInsertCallback()
{
	m_InsertCallback = nullptr;
}

void LogProvider::fillList()
{
	contacts_list_h list = fetchLogList();
	contacts_record_h record = nullptr;

	CONTACTS_LIST_FOREACH(list, record) {
		m_Logs.push_back(LogPtr(new Log(record)));
	}

	contacts_list_destroy(list, false);
}

void LogProvider::fillRecordList(RecordList &recordList)
{
	contacts_list_h list = fetchLogList();
	contacts_record_h record = nullptr;

	CONTACTS_LIST_FOREACH(list, record) {
		recordList.push_back(record);
	}

	contacts_list_destroy(list, false);
}

size_t LogProvider::fillGroupList(LogIterator begin, LogIterator end)
{
	LogGroup *lastLogGroup = nullptr;
	if (!m_Groups.empty()) {
		lastLogGroup = m_Groups.back().get();
	}
	size_t newGroupsCount = 0;

	for (auto &&log : Utils::makeRange(begin, end)) {
		if (lastLogGroup && shouldGroupLogs(*log, lastLogGroup->getFirstLog())) {
			lastLogGroup->addLog(log.get());
		} else {
			lastLogGroup = new LogGroup(log.get());
			m_Groups.push_back(LogGroupPtr(lastLogGroup));
			++newGroupsCount;
		}
	}

	return newGroupsCount;
}

bool LogProvider::shouldGroupLogs(Log &log, Log &prevLog)
{
	int type = log.getType();
	return (type == prevLog.getType()
			&& type != CONTACTS_PLOG_TYPE_VOICE_INCOMING_UNSEEN
			&& type != CONTACTS_PLOG_TYPE_VOICE_INCOMING_SEEN
			&& strcmp(log.getNumber(), prevLog.getNumber()) == 0
			&& compareDate(log.getTime(), prevLog.getTime()));
}

bool LogProvider::compareDate(const tm &firstDate, const tm &secondDate)
{
	if (firstDate.tm_year == secondDate.tm_year &&
		firstDate.tm_mon == secondDate.tm_mon &&
		firstDate.tm_mday == secondDate.tm_mday) {
		return true;
	}
	return false;
}

bool LogProvider::mergeGroup(GroupIterator group)
{
	if (group == m_Groups.begin()) {
		return false;
	}
	GroupIterator prevGroup = group;
	--prevGroup;
	if (shouldGroupLogs((*group)->getFirstLog(), (*prevGroup)->getFirstLog())) {
		(*prevGroup)->addLogList(*(group->get()));
		(*prevGroup)->onChange();
		return true;
	}

	return false;
}

contacts_filter_h LogProvider::getFilter()
{
	contacts_filter_h filter = nullptr;
	contacts_filter_create(_contacts_phone_log._uri, &filter);

	contacts_filter_add_int(filter, _contacts_phone_log.log_type, CONTACTS_MATCH_GREATER_THAN_OR_EQUAL, CONTACTS_PLOG_TYPE_VOICE_INCOMING);
	contacts_filter_add_operator(filter, CONTACTS_FILTER_OPERATOR_AND);
	contacts_filter_add_int(filter, _contacts_phone_log.log_type, CONTACTS_MATCH_LESS_THAN_OR_EQUAL, CONTACTS_PLOG_TYPE_VIDEO_BLOCKED);

	return filter;
}

contacts_list_h LogProvider::fetchLogList()
{
	contacts_list_h list = nullptr;
	contacts_query_h query = nullptr;
	contacts_filter_h filter = getFilter();

	contacts_query_create(_contacts_phone_log._uri, &query);
	contacts_query_set_filter(query, filter);
	contacts_query_set_sort(query, _contacts_phone_log.log_time, true);
	contacts_db_get_records_with_query(query, 0, 0, &list);

	contacts_filter_destroy(filter);
	contacts_query_destroy(query);

	return list;
}

void LogProvider::onLogsChanged(const char *viewUri)
{
	LogIterator newBegin = updateLogs();
	updateGroups(newBegin, m_Logs.end());
}

LogProvider::LogIterator LogProvider::updateLogs()
{
	RecordList newLogList;
	fillRecordList(newLogList);
	RecordIterator newIt = newLogList.begin();
	LogIterator oldIt = m_Logs.begin();

	while (oldIt != m_Logs.end() && newIt != newLogList.end()) {
		int id = 0;
		contacts_record_get_int(*newIt, _contacts_phone_log.id, &id);

		if (id != (*oldIt)->getId()) {
			oldIt = m_Logs.erase(oldIt);
		} else {
			(*oldIt)->update(*newIt);
			++oldIt;
			++newIt;
		}
	}

	while (oldIt != m_Logs.end()) {
		oldIt = m_Logs.erase(oldIt);
	}

	LogIterator newFirst = m_Logs.end();
	if (newIt != newLogList.end()) {
		m_Logs.push_back(LogPtr(new Log(*newIt)));
		newFirst = --m_Logs.end();
		++newIt;
	}
	while (newIt != newLogList.end()) {
		m_Logs.push_back(LogPtr(new Log(*newIt)));
		++newIt;
	}

	return newFirst;
}

void LogProvider::updateGroups(LogIterator newBegin, LogIterator newEnd)
{
	size_t count = fillGroupList(newBegin, newEnd);
	int updateCount = m_Groups.size() - count;
	GroupIterator updateIt = m_Groups.begin();
	bool wasDeleted = false;

	while (updateCount != 0) {
		bool isEmpty = false;
		if ((*updateIt)->getLogList().empty()) {
			isEmpty = true;
			wasDeleted = true;
		} else if (wasDeleted) {
			isEmpty = mergeGroup(updateIt);
			wasDeleted = false;
		}

		(*updateIt)->onChange();
		updateIt = isEmpty ? m_Groups.erase(updateIt) : ++updateIt;
		--updateCount;
	}

	if (m_InsertCallback) {
		while (updateIt != m_Groups.end()) {
			m_InsertCallback(updateIt->get());
			++updateIt;
		}
	}
}

void LogProvider::onContactChanged(const char *viewUri)
{
	contacts_changed_e changeType;
	bool isImageChanged = false;

	contacts_list_h list = nullptr;
	contacts_db_get_changes_by_version(_contacts_contact_updated_info._uri, 0, m_DbVersion, &list, &m_DbVersion);

	contacts_record_h record = nullptr;
	CONTACTS_LIST_FOREACH(list, record) {
		int id = 0;
		contacts_record_get_int(record, _contacts_contact_updated_info.contact_id, &id);
		contacts_record_get_int(record, _contacts_contact_updated_info.type, (int *)&changeType);

		if (changeType == CONTACTS_CHANGE_UPDATED) {
			contacts_record_get_bool(record, _contacts_contact_updated_info.image_changed, &isImageChanged);

			contacts_record_h record = nullptr;
			contacts_db_get_record(_contacts_contact._uri, id, &record);

			char *name = nullptr;
			contacts_record_get_str_p(record, _contacts_contact.display_name, &name);

			for (auto &&group : m_Groups) {
				const Log &log = group->getFirstLog();

				if (log.getContactId() == id) {
					const char *logName = log.getName();
					group->updateLogList();

					if (strcmp(name, logName) != 0) {
						group->setChangedType(LogGroup::ChangeName);
					}

					if (isImageChanged) {
						group->setChangedType(LogGroup::ChangeImage);
					}
					group->onChange();
				}
			}
		}
	}

	contacts_list_destroy(list, true);
}

