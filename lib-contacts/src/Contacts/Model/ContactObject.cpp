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

#include "Contacts/Model/ContactObject.h"
#include "Contacts/Model/ContactFieldMetadata.h"
#include "Common/Database/RecordUtils.h"

using namespace Common::Database;
using namespace Contacts::Model;

ContactField *ContactObject::getFieldById(unsigned id) const
{
	for (auto &&field : *this) {
		if (field.getId() == id) {
			return &field;
		}
	}

	return nullptr;
}

unsigned ContactObject::getInterfaces() const
{
	return getObjectMetadata().interfaces;
}

int ContactObject::getRecordId() const
{
	return getRecordInt(getRecord(), getObjectMetadata().idPropId);
}

bool ContactObject::operator==(contacts_record_h record) const
{
	return compareRecordsInt(getRecord(), record, getObjectMetadata().idPropId);
}

const ContactObjectMetadata &ContactObject::getObjectMetadata() const
{
	return *(const ContactObjectMetadata *) ContactField::getMetadata().typeMetadata;
}

void ContactObject::onInitialize(contacts_record_h record)
{
	for (auto &&field : getObjectMetadata().fields) {
		addField(getChildRecord(record, field), field);
	}
}

void ContactObject::onUpdate(contacts_record_h record)
{
	for (auto &&field : *this) {
		field.update(getChildRecord(record, field.getMetadata()));
	}
}

contacts_record_h ContactObject::getChildRecord(contacts_record_h record,
		const ContactFieldMetadata &metadata)
{
	if (metadata.typeMetadata->type != TypeObject) {
		return record;
	}

	const char *uri = ((const ContactObjectMetadata *) metadata.typeMetadata)->uri;
	if (strcmp(uri, getObjectMetadata().uri) == 0) {
		/* Return the same record for a fake object which is actually a different
		   adaptor to the same record */
		return record;
	}

	contacts_record_h childRecord = nullptr;
	int err = contacts_record_get_child_record_at_p(record, metadata.propId, 0, &childRecord);
	if (err == CONTACTS_ERROR_NO_DATA) {
		contacts_record_create(uri, &childRecord);
		contacts_record_add_child_record(record, metadata.propId, childRecord);
	}

	return childRecord;
}
