/*
 * Copyright (c) 2015-2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "Contacts/List/Model/ContactRecordData.h"
#include "Contacts/ContactRecordChildIterator.h"
#include <cstring>

using namespace Contacts::List::Model;

ContactRecordData::ContactRecordData(Type type)
	: ContactData(type), m_Record(nullptr)
{
}

const char *ContactRecordData::getName() const
{
	return getValue(m_Record, FieldName);
}

const char *ContactRecordData::getNumber() const
{
	return getValue(m_Record, FieldNumber);
}

const char *ContactRecordData::getImagePath() const
{
	return getValue(m_Record, FieldImage);
}

bool ContactRecordData::compare(const char *str)
{
	return strstr(getName(), str); //Todo: Compare unicode strings
}

void ContactRecordData::initContactRecord(int id)
{
	contacts_db_get_record(_contacts_contact._uri, id, &m_Record);
}

void ContactRecordData::clearContactRecord()
{
	contacts_record_destroy(m_Record, true);
	m_Record = nullptr;
}

const contacts_record_h ContactRecordData::getContactRecord() const
{
	return m_Record;
}

const char *ContactRecordData::getValue(contacts_record_h record, Field field)
{
	static unsigned int properties[] {
		/* FieldName   = */ _contacts_contact.display_name,
		/* FieldNumber = */ _contacts_contact.number,
		/* FieldImage  = */ _contacts_contact.image_thumbnail_path
	};

	char *strValue = nullptr;

	if (field == FieldNumber) {
		auto numbers = Contacts::makeRange(record, properties[field]);
		for (auto &&number : numbers) {
			bool isDefault = false;
			contacts_record_get_bool(number, _contacts_number.is_default, &isDefault);

			if (isDefault) {
				contacts_record_get_str_p(number, _contacts_number.number, &strValue);
				break;
			}
		}
	} else {
		contacts_record_get_str_p(record, properties[field], &strValue);
	}

	return strValue;
}
