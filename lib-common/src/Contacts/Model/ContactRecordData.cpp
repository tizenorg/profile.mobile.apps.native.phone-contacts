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

#include "Contacts/Model/ContactRecordData.h"
#include "Contacts/ContactRecordChildIterator.h"
#include <cstring>

using namespace Contacts::Model;

ContactRecordData::ContactRecordData(Type type)
	: ContactData(type), m_Record(nullptr)
{
}

ContactRecordData::~ContactRecordData()
{
	contacts_record_destroy(m_Record, true);
	unsetChangedCallback();
}

void ContactRecordData::updateContactRecord(contacts_record_h record)
{
	contacts_record_destroy(m_Record, true);
	m_Record = record;
}

int ContactRecordData::getId() const
{
	return getContactId(m_Record);
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

const contacts_record_h ContactRecordData::getContactRecord() const
{
	return m_Record;
}

void ContactRecordData::setChangedCallback(DbChangeObserver::Callback callback)
{
	int id = getContactId(m_Record);
	m_Handle = DbChangeObserver::getInstance()->addCallback(id, callback);
}

void ContactRecordData::unsetChangedCallback()
{
	DbChangeObserver::getInstance()->removeCallback(getId(), m_Handle);
}

int ContactRecordData::getContactId(contacts_record_h record)
{
	int value = 0;
	contacts_record_get_int(record, _contacts_contact.id, &value);
	return value;
}

const char *ContactRecordData::getValue(contacts_record_h record, Field field)
{
	static unsigned int properties[] {
		/* FieldName   = */ _contacts_contact.display_name,
		/* FieldNumber = */ _contacts_contact.number,
		/* FieldImage  = */ _contacts_contact.image_thumbnail_path
	};

	char *value = nullptr;

	if (field == FieldNumber) {
		auto numbers = Contacts::makeRange(record, properties[field]);
		for (auto &&number : numbers) {
			bool isDefault = false;
			contacts_record_get_bool(number, _contacts_number.is_default, &isDefault);

			if (isDefault) {
				contacts_record_get_str_p(number, _contacts_number.number, &value);
				break;
			}
		}
	} else {
		contacts_record_get_str_p(record, properties[field], &value);
	}

	return value;
}

int ContactRecordData::getChanges(contacts_record_h oldContact, contacts_record_h newContact)
{
	auto isChanged = [](const char *str1, const char *str2) {
		return (str1 && str2)
				? (strcmp(str1, str2) != 0)
				: (str1 != str2);
	};

	int changes = ChangedNone;

	for (int i = FieldName; i < FieldMax; ++i) {
		auto fieldId = static_cast<Field>(i);
		const char *oldValue = getValue(oldContact, fieldId);
		const char *newValue = getValue(newContact, fieldId);
		int changedInfo = 1 << fieldId;

		if (isChanged(oldValue, newValue)) {
			changes |= changedInfo;
		}
	}

	return changes;
}

void ContactRecordData::onUpdate(contacts_record_h record)
{
	int changes = getChanges(m_Record, record);
	updateContactRecord(record);
	onUpdated(changes);
}
