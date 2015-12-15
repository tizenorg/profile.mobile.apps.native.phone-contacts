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

using namespace Logs::Model;

LogProvider::LogProvider()
{
	contacts_db_get_current_version(&m_DbVersion);
	contacts_db_add_changed_cb(_contacts_phone_log._uri, makeCallbackWithLastParam(&LogProvider::onLogChanged), this);
	contacts_db_add_changed_cb(_contacts_person._uri, makeCallbackWithLastParam(&LogProvider::onContactChanged), this);
	fillList();
}

void LogProvider::fillList()
{
	contacts_list_h list = nullptr;
	contacts_db_get_all_records(_contacts_person_phone_log._uri, 0, 0, &list);
	contacts_record_h record = nullptr;

	CONTACTS_LIST_FOREACH(list, record) {
		LogPtr log = LogPtr(new LogRecord(record));

		if (m_AllLogs.size() != 0) {
			LogPtr lastLog = m_AllLogs.back();

			if (lastLog->getType() == log->getType() && strcmp(lastLog->getNumber(), log->getNumber()) == 0) {
				log = groupLogs(std::move(log), std::move(lastLog));
			}
		}
		m_AllLogs.push_back(log);
	}
}

LogGroupPtr LogProvider::groupLogs(LogPtr log, LogPtr lastLog)
{
	LogGroupPtr logGroup = nullptr;
	if (lastLog->isGroup()) {
		logGroup = std::static_pointer_cast<LogGroup>(lastLog);
		logGroup->addLog(std::move(log));
	} else {
		logGroup = LogGroupPtr(new LogGroup());
		logGroup->addLog(std::move(lastLog));
		m_AllLogs.pop_back();
		logGroup->addLog(std::move(log));
	}
	return logGroup;
}

const LogList &LogProvider::getLogsList()
{
	return m_AllLogs;
}

void LogProvider::onLogChanged(const char *viewUri)
{
	contacts_list_h list = nullptr;
	contacts_db_get_all_records(_contacts_person_phone_log._uri, 0, 0, &list);
	contacts_record_h record = nullptr;

	CONTACTS_LIST_FOREACH(list, record) {
		/*
		TODO
		*/
	}
	contacts_list_destroy(list, true);
}

void LogProvider::onContactChanged(const char *viewUri)
{
	contacts_list_h changes = nullptr;
	contacts_db_get_changes_by_version(_contacts_contact_updated_info._uri, 0, m_DbVersion, &changes, &m_DbVersion);

	contacts_record_h record = nullptr;
	CONTACTS_LIST_FOREACH(changes, record) {
		/*
		TODO
		*/
	}
	contacts_list_destroy(changes, true);
}
