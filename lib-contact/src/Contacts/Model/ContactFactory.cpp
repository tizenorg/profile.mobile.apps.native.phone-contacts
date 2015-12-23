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

#include "Contacts/Model/ContactFactory.h"
#include "Contacts/Model/ContactFieldMetadata.h"

#include "Contacts/Model/ContactArray.h"
#include "Contacts/Model/ContactDateField.h"
#include "Contacts/Model/ContactEnumField.h"
#include "Contacts/Model/ContactTextField.h"
#include "Contacts/Model/ContactTypedObject.h"

#include "Contacts/Model/ContactName.h"
#include "Contacts/Model/ContactPhoneticName.h"

using namespace Contacts::Model;

ContactFieldPtr ContactFactory::createField(contacts_record_h record,
		const ContactFieldMetadata &metadata)
{
	ContactField *field = nullptr;
	switch(metadata.typeMetadata->type) {
		case TypeEnum:
			field = new ContactEnumField(record, metadata); break;
		case TypeText:
			field = new ContactTextField(record, metadata); break;
		case TypeDate:
			field = new ContactDateField(record, metadata); break;
		case TypeArray:
			field = new ContactArray(record, metadata); break;
		case TypeObject:
		{
			record = getObjectRecord(record, metadata);

			unsigned subType = metadata.typeMetadata->subType;
			if (subType & ObjectTyped) {
				field = new ContactTypedObject(record, metadata);
			} else if (subType & ObjectCompound) {
				switch (metadata.id) {
					case FieldName:
						field = new ContactName(record, metadata);
						break;
					case FieldPhoneticName:
						field = new ContactPhoneticName(record, metadata);
						break;
				}
			} else {
				field = new ContactObject(record, metadata);
			}
		}
			break;
	}

	return ContactFieldPtr(field);
}

contacts_record_h ContactFactory::getObjectRecord(contacts_record_h record,
		const ContactFieldMetadata &metadata)
{
	contacts_record_h childRecord = nullptr;
	int err = contacts_record_get_child_record_at_p(record, metadata.propId, 0, &childRecord);

	if (err == CONTACTS_ERROR_NO_DATA) {
		const char *uri = ((const ContactObjectMetadata *) metadata.typeMetadata)->uri;
		contacts_record_create(uri, &childRecord);
		contacts_record_add_child_record(record, metadata.propId, childRecord);
	}

	return childRecord ? childRecord : record;
}
