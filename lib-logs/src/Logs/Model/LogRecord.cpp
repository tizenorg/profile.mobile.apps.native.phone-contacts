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

#include "Logs/Model/LogRecord.h"
#include "Utils/Logger.h"

using namespace Logs::Model;

LogRecord::LogRecord(contacts_record_h record)
	: m_LogRecord(record), m_ContactRecord(getContactRecord())
{
}

LogRecord::~LogRecord()
{
	contacts_record_destroy(m_LogRecord, true);
	contacts_record_destroy(m_ContactRecord, true);
}

bool LogRecord::isGroup() const
{
	return false;
}

const contacts_record_h LogRecord::getLogRecord() const
{
	return m_LogRecord;
}

const char *LogRecord::getName() const
{
	char *name = nullptr;
	contacts_record_get_str_p(m_ContactRecord, _contacts_person.display_name, &name);
	return name;
}

const char *LogRecord::getNumber() const
{
	char *number = nullptr;
	contacts_record_get_str_p(m_LogRecord, _contacts_phone_log.address, &number);
	return number;
}

const char *LogRecord::getImagePath() const
{
	char *path = nullptr;
	contacts_record_get_str_p(m_ContactRecord, _contacts_person.image_thumbnail_path, &path);
	return path;
}

struct tm LogRecord::getTime() const
{
	int time = 0;
	contacts_record_get_int(m_LogRecord, _contacts_phone_log.log_time, &time);

	time_t logTime = time;
	struct tm logDate;

	localtime_r(&logTime, &logDate);
	return logDate;
}

int LogRecord::getType() const
{
	int type = CONTACTS_PLOG_TYPE_NONE;
	contacts_record_get_int(m_LogRecord, _contacts_phone_log.log_type, &type);

	return type;
}

int LogRecord::getId() const
{
	int id = 0;
	contacts_record_get_int(m_LogRecord, _contacts_phone_log.id, &id);
	return id;
}

int LogRecord::getPersonId() const
{
	int id = 0;
	contacts_record_get_int(m_LogRecord, _contacts_phone_log.person_id, &id);
	return id;
}

contacts_record_h LogRecord::getContactRecord()
{
	contacts_record_h record = nullptr;
	contacts_db_get_record(_contacts_person._uri, getPersonId(), &record);
	return record;
}

void LogRecord::setContactChangeCallback(ContactChangeCallback callback)
{
	m_ContactChangeCallback = callback;
}

void LogRecord::unsetContactChangeCallback()
{
	m_ContactChangeCallback = nullptr;
}

void LogRecord::callContactChangeCallback(LogPtr log, contacts_changed_e type)
{
	m_ContactChangeCallback(log, type);
}
