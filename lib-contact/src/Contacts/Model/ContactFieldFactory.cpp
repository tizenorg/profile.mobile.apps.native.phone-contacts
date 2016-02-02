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

#include "Contacts/Model/ContactFieldFactory.h"
#include "Contacts/Model/ContactFieldMetadata.h"

#include "Contacts/Model/ContactArray.h"
#include "Contacts/Model/ContactBoolField.h"
#include "Contacts/Model/ContactDateField.h"
#include "Contacts/Model/ContactEnumField.h"
#include "Contacts/Model/ContactTextField.h"
#include "Contacts/Model/ContactTypedObject.h"

#include "Contacts/Model/ContactName.h"
#include "Contacts/Model/ContactPhoneticName.h"

using namespace Contacts::Model;

ContactFieldPtr ContactFieldFactory::createField(ContactFieldContainer *parent,
		const ContactFieldMetadata &metadata)
{
	ContactField *field = nullptr;
	switch(metadata.typeMetadata->type) {
		case TypeBool:
			field = new ContactBoolField(parent, metadata); break;
		case TypeEnum:
			field = new ContactEnumField(parent, metadata); break;
		case TypeText:
			field = new ContactTextField(parent, metadata); break;
		case TypeDate:
			field = new ContactDateField(parent, metadata); break;
		case TypeArray:
			field = new ContactArray(parent, metadata); break;
		case TypeObject:
		{
			unsigned subType = metadata.typeMetadata->subType;
			if (subType & ObjectTyped) {
				field = new ContactTypedObject(parent, metadata);
			} else if (subType & ObjectCompound) {
				switch (metadata.id) {
					case FieldName:
						field = new ContactName(parent, metadata);
						break;
					case FieldPhoneticName:
						field = new ContactPhoneticName(parent, metadata);
						break;
				}
			} else {
				field = new ContactObject(parent, metadata);
			}
		}
			break;
	}

	return ContactFieldPtr(field);
}
