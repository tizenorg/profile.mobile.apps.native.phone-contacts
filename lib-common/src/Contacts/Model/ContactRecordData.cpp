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
#include "Common/Database/ChildRecordIterator.h"
#include "Common/Database/RecordUtils.h"
#include "Utils/String.h"

using namespace Common::Database;
using namespace Contacts::Model;

ContactRecordData::ContactRecordData(contacts_record_h record)
	: ContactData(TypeContact), m_Record(record)
{
}

ContactRecordData::~ContactRecordData()
{
	contacts_record_destroy(m_Record, true);
}

int ContactRecordData::getId() const
{
	return getRecordInt(m_Record, _contacts_contact.id);
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

contacts_record_h ContactRecordData::getRecord() const
{
	return m_Record;
}

void ContactRecordData::updateRecord(contacts_record_h record)
{
	contacts_record_destroy(m_Record, true);
	m_Record = record;
}

int ContactRecordData::getChanges(contacts_record_h newContact)
{
	int changes = ChangedNone;

	for (int i = FieldName; i < FieldMax; ++i) {
		auto fieldId = static_cast<Field>(i);
		const char *oldValue = getValue(m_Record, fieldId);
		const char *newValue = getValue(newContact, fieldId);

		if (!Utils::safeCmp(oldValue, newValue)) {
			changes |= (1 << fieldId);
		}
	}

	return changes;
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
		auto numbers = makeRange(record, properties[field]);
		for (auto &&number : numbers) {
			bool isDefault = getRecordBool(number, _contacts_number.is_default);
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

void ContactRecordData::onUpdate(contacts_record_h record)
{
	int changes = getChanges(record);
	updateRecord(record);
	onUpdated(changes);
}
