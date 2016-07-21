/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "Contacts/List/SharePopup.h"

#include "Contacts/Model/ContactArray.h"
#include "Contacts/Model/ContactDateField.h"
#include "Contacts/Model/ContactTypedObject.h"
#include "Contacts/Model/ContactFieldMetadata.h"
#include "Common/Database/Queries.h"
#include "Common/Strings.h"
#include "Utils/Logger.h"

#include "App/AppControlRequest.h"

#include <app_i18n.h>

#define DATE_BUFFER_SIZE 32

using namespace Common;
using namespace Common::Database;
using namespace Contacts::List;
using namespace Contacts::Model;
using namespace Ux;

SharePopup::SharePopup(int personId)
	: m_PersonId(personId)
{
}

void SharePopup::onCreated()
{
	setTitle("IDS_PB_HEADER_SHARE");
	addItem("IDS_PB_OPT_VCARD_FILE_HVCF", std::bind(&SharePopup::shareAsVcard, this));
	addItem("IDS_PB_OPT_TEXT", std::bind(&SharePopup::shareAsText, this));
}

void SharePopup::shareAsVcard()
{
	std::string idStr = std::to_string(m_PersonId);
	const char *id = idStr.c_str();

	App::AppControl request = App::requestMultiShareContacts(&id, 1);
	request.launch();
	request.detach();
}

void SharePopup::shareAsText()
{
	App::AppControl request = App::requestShareText(prepareText(m_PersonId).c_str());
	request.launch();
	request.detach();
}

std::string SharePopup::prepareText(int personId)
{
	std::string text;
	contacts_record_h record = nullptr;
	contacts_db_get_record(_contacts_contact._uri, getDisplayContactId(m_PersonId), &record);

	ContactObject contact(nullptr, *getContactMetadata(ObjectTypeContact));
	contact.initialize(record);
	formatObject(text, contact);

	contacts_record_destroy(record, true);
	return text;
}

void SharePopup::formatObject(std::string &text, ContactObject &object)
{
	static bool isFieldVisible[] = {
		/* [FieldFavorite]     = */ false,
		/* [FieldDisplayName]  = */ false,
		/* [FieldImage]        = */ false,
		/* [FieldName]         = */ true,
		/* [FieldPhoneticName] = */ true,
		/* [FieldCompany]      = */ true,
		/* [FieldNumber]       = */ true,
		/* [FieldEmail]        = */ true,
		/* [FieldAddress]      = */ true,
		/* [FieldUrl]          = */ true,
		/* [FieldMessenger]    = */ true,
		/* [FieldEvent]        = */ true,
		/* [FieldNote]         = */ true,
		/* [FieldNickname]     = */ true,
		/* [FieldRelationship] = */ true,
		/* [FieldRingtone]     = */ false
	};

	bool isEmpty = true;
	for (auto &&field : object) {
		if (object.getSubType() == ObjectTypeContact && !isFieldVisible[field.getId()]) {
			continue;
		}

		if (object.getInterfaces() & InterfaceTypedObject) {
			formatTypedObject(text, object);
		}

		switch (field.getType()) {
			case TypeArray:
			{
				for (auto &&element : field.cast<ContactArray>()) {
					formatObject(text, element.cast<ContactObject>());
				}
				break;
			}
			case TypeObject:
			{
				formatObject(text, field.cast<ContactObject>());
				break;
			}
			case TypeText:
			{
				const char *value = field.cast<ContactTextField>().getValue();
				if (value) {
					if (!isEmpty) {
						text.append(" ");
					}

					isEmpty = false;
					text.append(value);
				}
				break;
			}
			case TypeDate:
			{
				isEmpty = false;
				tm date = field.cast<ContactDateField>().getValue();
				char buffer[DATE_BUFFER_SIZE];
				strftime(buffer, sizeof(buffer), "%x", &date);

				text.append(buffer);
				break;
			}
			default: break;
		}
	}

	if (!isEmpty) {
		text.append("\n");
	}
}


void SharePopup::formatTypedObject(std::string &text, ContactObject &object)
{
	ContactTypedObject &typedObject = object.cast<ContactTypedObject>();
	ContactEnumField &typeField = typedObject.getTypeField();
	if (typeField.hasCustomValue()) {
		const char *value = typedObject.getLabelField().getValue();
		if (value) {
			text.append(value).append(" ");
		}
	} else {
		const char *value = getEnumValueName(EnumType(typeField.getSubType()), typeField.getValue());
		if (value) {
			text.append(_(value)).append(" ");
		}
	}
}
