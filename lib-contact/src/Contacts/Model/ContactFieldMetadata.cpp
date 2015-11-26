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

#include "Contacts/Model/ContactFieldMetadata.h"
#include "Contacts/Model/ContactFields.h"
#include "Contacts/Model/ContactTypes.h"

#include <contacts.h>

#define COUNT(array) sizeof(array) / sizeof(*array)
#define ARRAY(array) array, COUNT(array)
#define UPCAST(metadata) (const ContactFieldMetadata *) metadata

using namespace Contacts;
using namespace Contacts::Model;

namespace
{

/******************************** Text types **********************************/
const ContactFieldMetadata contactRegularText = { TypeText, TextTypeRegular };
const ContactFieldMetadata contactNumberText = { TypeText, TextTypeNumber };
const ContactFieldMetadata contactEmailText = { TypeText, TextTypeEmail };
const ContactFieldMetadata contactUrlText = { TypeText, TextTypeUrl };

/******************************** Date type ***********************************/
const ContactFieldMetadata contactDate = { TypeText, 0 };

/****************************** Company Object ********************************/
const ContactObjectField contactCompanyFields[] = {
	{ CompanyFieldName,     &contactRegularText },
	{ CompanyFieldJobTitle, &contactRegularText }
};
const ContactObjectMetadata contactCompany = {
	TypeObject, ObjectTypeCompany, ARRAY(contactCompanyFields)
};

/******************************* Name Object **********************************/
const ContactObjectField contactNameFields[] = {
	{ NameFieldTitle,  &contactRegularText },
	{ NameFieldFirst,  &contactRegularText },
	{ NameFieldMiddle, &contactRegularText },
	{ NameFieldLast,   &contactRegularText },
	{ NameFieldSuffix, &contactRegularText }
};
const ContactObjectMetadata contactName = {
	TypeObject, ObjectTypeName, ARRAY(contactNameFields)
};

/*************************** Phonetic Name Object *****************************/
const ContactObjectField contactPhNameFields[] = {
	{ PhoneticNameFieldFirst,  &contactRegularText },
	{ PhoneticNameFieldMiddle, &contactRegularText },
	{ PhoneticNameFieldLast,   &contactRegularText }
};
const ContactObjectMetadata contactPhName = {
	TypeObject, ObjectTypePhoneticName, ARRAY(contactPhNameFields)
};

/******************************** Note Object *********************************/
const ContactObjectField contactNoteFields[] = {
	{ FieldNote, &contactRegularText },
};
const ContactObjectMetadata contactNote = {
	TypeObject, ObjectTypeNote, ARRAY(contactNoteFields)
};

/***************************** Nickname Object ********************************/
const ContactObjectField contactNickFields[] = {
	{ FieldNickname, &contactRegularText },
};
const ContactObjectMetadata contactNick = {
	TypeObject, ObjectTypeNickname, ARRAY(contactNickFields)
};

/******************************* Image Object *********************************/
const int contactImageTypeValues[] = {
	CONTACTS_IMAGE_TYPE_OTHER,
	CONTACTS_IMAGE_TYPE_CUSTOM
};
const ContactEnumMetadata contactImageType = {
	TypeEnum, EnumTypeImage, ARRAY(contactImageTypeValues),
	CONTACTS_IMAGE_TYPE_OTHER, CONTACTS_IMAGE_TYPE_CUSTOM
};
const ContactObjectField contactImageFields[] = {
	{ FieldImage, &contactRegularText },
	{ FieldImage, UPCAST(&contactImageType) }
};
const ContactObjectMetadata contactImage = {
	TypeObject, ObjectTypeImage, ARRAY(contactImageFields)
};

/****************************** Number Object *********************************/
const int contactNumberTypeValues[] = {
	CONTACTS_NUMBER_TYPE_CELL,
	CONTACTS_NUMBER_TYPE_VOICE | CONTACTS_NUMBER_TYPE_HOME,
	CONTACTS_NUMBER_TYPE_VOICE | CONTACTS_NUMBER_TYPE_WORK,
	CONTACTS_NUMBER_TYPE_MAIN,
	CONTACTS_NUMBER_TYPE_FAX | CONTACTS_NUMBER_TYPE_WORK,
	CONTACTS_NUMBER_TYPE_FAX | CONTACTS_NUMBER_TYPE_HOME,
	CONTACTS_NUMBER_TYPE_PAGER,
	CONTACTS_NUMBER_TYPE_OTHER,
	CONTACTS_NUMBER_TYPE_CUSTOM
};
const ContactEnumMetadata contactNumberType = {
	TypeEnum, EnumTypeNumber, ARRAY(contactNumberTypeValues),
	CONTACTS_NUMBER_TYPE_CELL, CONTACTS_NUMBER_TYPE_CUSTOM
};
const ContactObjectField contactNumberFields[] = {
	{ FieldNumber, &contactNumberText },
	{ FieldNumber, UPCAST(&contactNumberType) }
};
const ContactObjectMetadata contactNumber = {
	TypeObject, ObjectTypeNumber, ARRAY(contactNumberFields)
};

/******************************* Email Object *********************************/
const int contactEmailTypeValues[] = {
	CONTACTS_EMAIL_TYPE_HOME,
	CONTACTS_EMAIL_TYPE_WORK,
	CONTACTS_EMAIL_TYPE_OTHER,
	CONTACTS_EMAIL_TYPE_CUSTOM
};
const ContactEnumMetadata contactEmailType = {
	TypeEnum, EnumTypeEmail, ARRAY(contactEmailTypeValues),
	CONTACTS_EMAIL_TYPE_HOME, CONTACTS_EMAIL_TYPE_CUSTOM
};
const ContactObjectField contactEmailFields[] = {
	{ FieldEmail, &contactEmailText },
	{ FieldEmail, UPCAST(&contactEmailType) }
};
const ContactObjectMetadata contactEmail = {
	TypeObject, ObjectTypeEmail, ARRAY(contactEmailFields)
};

/****************************** Address Object ********************************/
const int contactAddrTypeValues[] = {
	CONTACTS_ADDRESS_TYPE_HOME,
	CONTACTS_ADDRESS_TYPE_WORK,
	CONTACTS_ADDRESS_TYPE_OTHER,
	CONTACTS_ADDRESS_TYPE_CUSTOM
};
const ContactEnumMetadata contactAddrType = {
	TypeEnum, EnumTypeAddress, ARRAY(contactAddrTypeValues),
	CONTACTS_ADDRESS_TYPE_HOME, CONTACTS_ADDRESS_TYPE_CUSTOM
};
const ContactObjectField contactAddrFields[] = {
	{ FieldAddress, &contactRegularText },
	{ FieldAddress, UPCAST(&contactAddrType) }
};
const ContactObjectMetadata contactAddr = {
	TypeObject, ObjectTypeAddress, ARRAY(contactAddrFields)
};

/******************************* Event Object *********************************/
const int contactEventTypeValues[] = {
	CONTACTS_EVENT_TYPE_BIRTH,
	CONTACTS_EVENT_TYPE_ANNIVERSARY,
	CONTACTS_EVENT_TYPE_OTHER,
	CONTACTS_EVENT_TYPE_CUSTOM
};
const ContactEnumMetadata contactEventType = {
	TypeEnum, EnumTypeEvent,
	ARRAY(contactEventTypeValues), CONTACTS_EVENT_TYPE_BIRTH, CONTACTS_EVENT_TYPE_CUSTOM
};
const ContactObjectField contactEventFields[] = {
	{ FieldEvent, &contactDate },
	{ FieldEvent, UPCAST(&contactEventType) }
};
const ContactObjectMetadata contactEvent = {
	TypeObject, ObjectTypeEvent, ARRAY(contactEventFields)
};

/**************************** Relationship Object *****************************/
const int contactRelTypeValues[] = {
	CONTACTS_RELATIONSHIP_TYPE_ASSISTANT,
	CONTACTS_RELATIONSHIP_TYPE_BROTHER,
	CONTACTS_RELATIONSHIP_TYPE_CHILD,
	CONTACTS_RELATIONSHIP_TYPE_DOMESTIC_PARTNER,
	CONTACTS_RELATIONSHIP_TYPE_FATHER,
	CONTACTS_RELATIONSHIP_TYPE_FRIEND,
	CONTACTS_RELATIONSHIP_TYPE_MANAGER,
	CONTACTS_RELATIONSHIP_TYPE_MOTHER,
	CONTACTS_RELATIONSHIP_TYPE_PARENT,
	CONTACTS_RELATIONSHIP_TYPE_PARTNER,
	CONTACTS_RELATIONSHIP_TYPE_REFERRED_BY,
	CONTACTS_RELATIONSHIP_TYPE_RELATIVE,
	CONTACTS_RELATIONSHIP_TYPE_SISTER,
	CONTACTS_RELATIONSHIP_TYPE_SPOUSE,
	CONTACTS_RELATIONSHIP_TYPE_CUSTOM
};
const ContactEnumMetadata contactRelType = {
	TypeEnum, EnumTypeRelationship, ARRAY(contactRelTypeValues),
	CONTACTS_RELATIONSHIP_TYPE_ASSISTANT, CONTACTS_RELATIONSHIP_TYPE_CUSTOM
};
const ContactObjectField contactRelFields[] = {
	{ FieldRelationship, &contactRegularText },
	{ FieldRelationship, UPCAST(&contactRelType) }
};
const ContactObjectMetadata contactRel = {
	TypeObject, ObjectTypeRelationship, ARRAY(contactRelFields)
};

/******************************** Url Object **********************************/
const int contactUrlTypeValues[] = {
	CONTACTS_URL_TYPE_HOME,
	CONTACTS_URL_TYPE_WORK,
	CONTACTS_URL_TYPE_OTHER,
	CONTACTS_URL_TYPE_CUSTOM
};
const ContactEnumMetadata contactUrlType = {
	TypeEnum, EnumTypeUrl,
	ARRAY(contactUrlTypeValues), CONTACTS_URL_TYPE_HOME, CONTACTS_URL_TYPE_CUSTOM
};
const ContactObjectField contactUrlFields[] = {
	{ FieldUrl, &contactUrlText },
	{ FieldUrl, UPCAST(&contactUrlType) }
};
const ContactObjectMetadata contactUrl = {
	TypeObject, ObjectTypeUrl, ARRAY(contactUrlFields)
};

/***************************** Messenger Object *******************************/
const int contactMsgTypeValues[] = {
	CONTACTS_MESSENGER_TYPE_AIM,
	CONTACTS_MESSENGER_TYPE_YAHOO,
	CONTACTS_MESSENGER_TYPE_SKYPE,
	CONTACTS_MESSENGER_TYPE_QQ,
	CONTACTS_MESSENGER_TYPE_GOOGLE,
	CONTACTS_MESSENGER_TYPE_ICQ,
	CONTACTS_MESSENGER_TYPE_JABBER,
	CONTACTS_MESSENGER_TYPE_CUSTOM
};
const ContactEnumMetadata contactMsgType = {
	TypeEnum, EnumTypeMessenger, ARRAY(contactMsgTypeValues),
	CONTACTS_MESSENGER_TYPE_AIM, CONTACTS_MESSENGER_TYPE_CUSTOM
};
const ContactObjectField contactMsgFields[] = {
	{ FieldMessenger, &contactRegularText },
	{ FieldMessenger, UPCAST(&contactMsgType) }
};
const ContactObjectMetadata contactMsg = {
	TypeObject, ObjectTypeMessenger, ARRAY(contactMsgFields)
};

/******************************* Array Fields *********************************/
const ContactArrayMetadata contactNumbers = {
	TypeArray, ObjectTypeNumber, UPCAST(&contactNumber)
};
const ContactArrayMetadata contactEmails = {
	TypeArray, ObjectTypeEmail, UPCAST(&contactEmail)
};
const ContactArrayMetadata contactAddrs = {
	TypeArray, ObjectTypeAddress, UPCAST(&contactAddr)
};
const ContactArrayMetadata contactEvents = {
	TypeArray, ObjectTypeEvent, UPCAST(&contactEvent)
};
const ContactArrayMetadata contactRels = {
	TypeArray, ObjectTypeRelationship, UPCAST(&contactRel)
};
const ContactArrayMetadata contactUrls = {
	TypeArray, ObjectTypeUrl, UPCAST(&contactUrl)
};
const ContactArrayMetadata contactMsgs = {
	TypeArray, ObjectTypeMessenger, UPCAST(&contactMsg)
};

/****************************** Contact Object ********************************/
const ContactObjectField contactFields[] = {
	{ FieldImage,        UPCAST(&contactImage) },
	{ FieldName,         UPCAST(&contactName) },
	{ FieldNumber,       UPCAST(&contactNumbers) },
	{ FieldEmail,        UPCAST(&contactEmails) },
	{ FieldNote,         UPCAST(&contactNote) },
	{ FieldAddress,      UPCAST(&contactAddrs) },
	{ FieldEvent,        UPCAST(&contactEvents) },
	{ FieldRelationship, UPCAST(&contactRels) },
	{ FieldUrl,          UPCAST(&contactUrls) },
	{ FieldMessenger,    UPCAST(&contactMsgs) },
	{ FieldNickname,     UPCAST(&contactNick) },
	{ FieldCompany,      UPCAST(&contactCompany) },
	{ FieldPhoneticName, UPCAST(&contactPhName) }
};
const ContactObjectMetadata contact = {
	TypeObject, ObjectTypeContact, ARRAY(contactFields)
};

/******************************* Subtype maps *********************************/
const ContactFieldMetadata *textTypes[] = {
	/* [TextTypeRegular] = */ &contactRegularText,
	/* [TextTypeNumber]  = */ &contactNumberText,
	/* [TextTypeEmail]   = */ &contactEmailText,
	/* [TextTypeUrl]     = */ &contactUrlText
};

const ContactEnumMetadata *enums[] = {
	/* [EnumTypeImage]          = */ &contactImageType,
	/* [EnumTypeNumber]         = */ &contactNumberType,
	/* [EnumTypeEmail]          = */ &contactEmailType,
	/* [EnumTypeAddress]        = */ &contactAddrType,
	/* [EnumTypeEvent]          = */ &contactEventType,
	/* [EnumTypeRelationship]   = */ &contactRelType,
	/* [EnumTypeUrl]            = */ &contactUrlType,
	/* [EnumTypeMessenger]      = */ &contactMsgType
};

const ContactArrayMetadata *arrays[] = {
	/* [ObjectTypeContact]      = */ nullptr,
	/* [ObjectTypeMyProfile]    = */ nullptr,
	/* [ObjectTypeImage]        = */ nullptr,
	/* [ObjectTypeName]         = */ nullptr,
	/* [ObjectTypeNumber]       = */ &contactNumbers,
	/* [ObjectTypeEmail]        = */ &contactEmails,
	/* [ObjectTypeNote]         = */ nullptr,
	/* [ObjectTypeAddress]      = */ &contactAddrs,
	/* [ObjectTypeEvent]        = */ &contactEvents,
	/* [ObjectTypeRelationship] = */ &contactRels,
	/* [ObjectTypeUrl]          = */ &contactUrls,
	/* [ObjectTypeMessenger]    = */ &contactMsgs
};

const ContactObjectMetadata *objects[] = {
	/* [ObjectTypeContact]      = */ &contact,
	/* [ObjectTypeMyProfile]    = */ &contact,
	/* [ObjectTypeImage]        = */ &contactImage,
	/* [ObjectTypeName]         = */ &contactName,
	/* [ObjectTypeNumber]       = */ &contactNumber,
	/* [ObjectTypeEmail]        = */ &contactEmail,
	/* [ObjectTypeNote]         = */ &contactNote,
	/* [ObjectTypeAddress]      = */ &contactAddr,
	/* [ObjectTypeEvent]        = */ &contactEvent,
	/* [ObjectTypeRelationship] = */ &contactRel,
	/* [ObjectTypeUrl]          = */ &contactUrl,
	/* [ObjectTypeMessenger]    = */ &contactMsg,
	/* [ObjectTypeNickname]     = */ &contactNick,
	/* [ObjectTypeCompany]      = */ &contactCompany,
	/* [ObjectTypePhoneticName] = */ &contactPhName
};

} /* Anonymous namespace */

const ContactFieldMetadata *Model::getContactFieldMetadata(ContactFieldType fieldType,
		unsigned fieldSubType)
{
	const ContactFieldMetadata *metadata = nullptr;
	switch (fieldType) {
		case TypeText:
			if (fieldSubType < COUNT(textTypes)) {
				metadata = UPCAST(textTypes[fieldSubType]);
			}
			break;
		case TypeDate:
			metadata = &contactDate;
			break;
		case TypeEnum:
		case TypeLabel:
			if (fieldSubType < COUNT(enums)) {
				metadata = UPCAST(enums[fieldSubType]);
			}
			break;
		case TypeArray:
			if (fieldSubType < COUNT(arrays)) {
				metadata = UPCAST(arrays[fieldSubType]);
			}
			break;
		case TypeObject:
			if (fieldSubType < COUNT(objects)) {
				metadata = UPCAST(objects[fieldSubType]);
			}
			break;
	}

	return metadata;
}
