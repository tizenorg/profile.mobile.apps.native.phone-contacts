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
#include "Contacts/Model/ContactObject.h"
#include "Contacts/Model/ContactTextField.h"

using namespace Contacts::Model;

ContactObject ContactFactory::createContact(contacts_record_h record)
{
	return ContactObject(record, 0, getContactFieldMetadata(TypeObject, ObjectTypeContact));
}
ContactObject ContactFactory::createMyProfile(contacts_record_h record)
{
	return ContactObject(record, 0, getContactFieldMetadata(TypeObject, ObjectTypeMyProfile));
}

ContactFieldPtr ContactFactory::createField(contacts_record_h record,
		unsigned fieldId, const ContactFieldMetadata *metadata)
{
	ContactField *field = nullptr;
	switch(metadata->type) {
		case TypeEnum:
		case TypeLabel:
			field = new ContactEnumField(record, fieldId, metadata); break;
		case TypeText:
			field = new ContactTextField(record, fieldId, metadata); break;
		case TypeDate:
			field = new ContactDateField(record, fieldId, metadata); break;
		case TypeArray:
			field = new ContactArray(record, fieldId, metadata); break;
		case TypeObject:
			record = getObjectRecord(record, fieldId, metadata);
			field = new ContactObject(record, fieldId, metadata); break;
	}

	return ContactFieldPtr(field);
}

contacts_record_h ContactFactory::getObjectRecord(contacts_record_h record,
		unsigned fieldId, const ContactFieldMetadata *metadata)
{
	contacts_record_h childRecord = nullptr;
	unsigned valueId = ContactField::getPropertyId(fieldId, metadata->type);

	int err = contacts_record_get_child_record_at_p(record, valueId, 0, &childRecord);
	if (err == CONTACTS_ERROR_NO_DATA) {
		const char *uri = ContactField::getUri(metadata->subType);
		contacts_record_create(uri, &childRecord);
		contacts_record_add_child_record(record, valueId, childRecord);
	}

	return childRecord ? childRecord : record;
}
