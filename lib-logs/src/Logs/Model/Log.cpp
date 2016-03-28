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

#include "Logs/Model/Log.h"
#include "Logs/Model/LogGroup.h"
#include "Contacts/Utils.h"
#include "Utils/Logger.h"

using namespace Logs::Model;

Log::Log(contacts_record_h record)
	: m_LogRecord(record), m_ContactRecord(getContactRecord()), m_Group(nullptr)
{
}

Log::~Log()
{
	if (m_Group) {
		m_Group->removeLog(this);
	}
	contacts_record_destroy(m_LogRecord, true);
	if (m_ContactRecord) {
		contacts_record_destroy(m_ContactRecord, true);
	}
}

const char *Log::getName() const
{
	char *name = nullptr;
	if (m_ContactRecord) {
		contacts_record_get_str_p(m_ContactRecord, _contacts_person.display_name, &name);
	}
	return name;
}

const char *Log::getNumber() const
{
	return Contacts::getRecordStr(m_LogRecord, _contacts_phone_log.address);
}

const char *Log::getImagePath() const
{
	char *path = nullptr;
	if (m_ContactRecord) {
		contacts_record_get_str_p(m_ContactRecord, _contacts_person.image_thumbnail_path, &path);
	}
	return path;
}

int Log::getType() const
{
	return Contacts::getRecordInt(m_LogRecord, _contacts_phone_log.log_type);
}

tm Log::getTime() const
{
	time_t time = 0;
	contacts_record_get_int(m_LogRecord, _contacts_phone_log.log_time, (int *) &time);
	return *localtime(&time);
}

int Log::getId() const
{
	return Contacts::getRecordInt(m_LogRecord, _contacts_phone_log.id);
}

int Log::getPersonId() const
{
	return Contacts::getRecordInt(m_LogRecord, _contacts_phone_log.person_id);
}

int Log::getContactId() const
{
	int id = 0;
	if (m_ContactRecord) {
		contacts_record_get_int(m_ContactRecord, _contacts_person.display_contact_id, &id);
	}
	return id;
}

time_t Log::getDuration() const
{
	return (time_t) Contacts::getRecordInt(m_LogRecord, _contacts_phone_log.extra_data1);
}

contacts_record_h Log::getContactRecord()
{
	contacts_record_h record = nullptr;
	int personId = getPersonId();
	if (personId > 0) {
		contacts_db_get_record(_contacts_person._uri, personId, &record);
	}
	return record;
}

void Log::setLogGroup(LogGroup *group)
{
	m_Group = group;
}

LogGroup *Log::getLogGroup() const
{
	return m_Group;
}

void Log::update(contacts_record_h record)
{
	int personId = 0;
	contacts_record_get_int(record, _contacts_phone_log.person_id, &personId);

	if (personId != getPersonId()) {
		contacts_record_destroy(m_LogRecord, true);
		m_LogRecord = record;
		m_Group->setChangedType(LogGroup::ChangeImage | LogGroup::ChangeName);
		updateContactRecord();
	} else {
		contacts_record_destroy(record, true);
	}
}

void Log::updateContactRecord()
{
	if (m_ContactRecord) {
		contacts_record_destroy(m_ContactRecord, true);
	}
	m_ContactRecord = getContactRecord();
}

void Log::remove()
{
	contacts_db_delete_record(_contacts_phone_log._uri, getId());
}
