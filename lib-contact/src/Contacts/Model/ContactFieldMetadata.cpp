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
#include "Utils/Range.h"

#include <contacts.h>

#define UPCAST(metadata) (const ContactTypeMetadata *) metadata

using namespace Contacts;
using namespace Contacts::Model;
using namespace Utils;

namespace
{

/******************************** Text types **********************************/
const ContactTypeMetadata contactRegularText = { TypeText, TextTypeRegular };
const ContactTypeMetadata contactNumberText = { TypeText, TextTypeNumber };
const ContactTypeMetadata contactEmailText = { TypeText, TextTypeEmail };
const ContactTypeMetadata contactUrlText = { TypeText, TextTypeUrl };

/******************************** Date type ***********************************/
const ContactTypeMetadata contactDate = { TypeText, 0 };

/****************************** Company Object ********************************/
const ContactFieldMetadata contactCompanyFields[] = {
	{ CompanyFieldName,     _contacts_company.name,      &contactRegularText },
	{ CompanyFieldJobTitle, _contacts_company.job_title, &contactRegularText }
};
const ContactObjectMetadata contactCompany = {
	TypeObject, ObjectTypeCompany, _contacts_company._uri, makeRange(contactCompanyFields)
};

/******************************* Name Object **********************************/
const ContactFieldMetadata contactNameFields[] = {
	{ NameFieldTitle,  _contacts_name.prefix,   &contactRegularText },
	{ NameFieldFirst,  _contacts_name.first,    &contactRegularText },
	{ NameFieldMiddle, _contacts_name.addition, &contactRegularText },
	{ NameFieldLast,   _contacts_name.last,     &contactRegularText },
	{ NameFieldSuffix, _contacts_name.suffix,   &contactRegularText }
};
const ContactObjectMetadata contactName = {
	TypeObject, ObjectTypeName, _contacts_name._uri, makeRange(contactNameFields)
};

/*************************** Phonetic Name Object *****************************/
const ContactFieldMetadata contactPhNameFields[] = {
	{ PhoneticNameFieldFirst,  _contacts_name.phonetic_first,  &contactRegularText },
	{ PhoneticNameFieldMiddle, _contacts_name.phonetic_middle, &contactRegularText },
	{ PhoneticNameFieldLast,   _contacts_name.phonetic_last,   &contactRegularText }
};
const ContactObjectMetadata contactPhName = {
	TypeObject, ObjectTypePhoneticName, _contacts_name._uri, makeRange(contactPhNameFields)
};

/******************************** Note Object *********************************/
const ContactFieldMetadata contactNoteFields[] = {
	{ FieldNote, _contacts_note.note, &contactRegularText },
};
const ContactObjectMetadata contactNote = {
	TypeObject, ObjectTypeNote, _contacts_note._uri, makeRange(contactNoteFields)
};

/***************************** Nickname Object ********************************/
const ContactFieldMetadata contactNickFields[] = {
	{ FieldNickname, _contacts_nickname.name, &contactRegularText },
};
const ContactObjectMetadata contactNick = {
	TypeObject, ObjectTypeNickname, _contacts_nickname._uri, makeRange(contactNickFields)
};

/******************************* Image Object *********************************/
const int contactImageTypeValues[] = {
	CONTACTS_IMAGE_TYPE_OTHER,
	CONTACTS_IMAGE_TYPE_CUSTOM
};
const ContactEnumMetadata contactImageType = {
	TypeEnum, EnumTypeImage, makeRange(contactImageTypeValues),
	CONTACTS_IMAGE_TYPE_OTHER, CONTACTS_IMAGE_TYPE_CUSTOM
};
const ContactFieldMetadata contactImageFields[] = {
	{ FieldImage, _contacts_image.path, &contactRegularText }
};
const ContactTypedObjectMetadata contactImage = {
	TypeObject, ObjectTypeImage, _contacts_image._uri, makeRange(contactImageFields),
	{ FieldImage, _contacts_image.type,  UPCAST(&contactImageType) },
	{ FieldImage, _contacts_image.label, &contactRegularText }
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
	TypeEnum, EnumTypeNumber, makeRange(contactNumberTypeValues),
	CONTACTS_NUMBER_TYPE_CELL, CONTACTS_NUMBER_TYPE_CUSTOM
};
const ContactFieldMetadata contactNumberFields[] = {
	{ FieldNumber, _contacts_number.number, &contactNumberText }
};
const ContactTypedObjectMetadata contactNumber = {
	TypeObject, ObjectTypeNumber, _contacts_number._uri, makeRange(contactNumberFields),
	{ FieldNumber, _contacts_number.type,  UPCAST(&contactNumberType) },
	{ FieldNumber, _contacts_number.label, &contactRegularText }
};

/******************************* Email Object *********************************/
const int contactEmailTypeValues[] = {
	CONTACTS_EMAIL_TYPE_HOME,
	CONTACTS_EMAIL_TYPE_WORK,
	CONTACTS_EMAIL_TYPE_OTHER,
	CONTACTS_EMAIL_TYPE_CUSTOM
};
const ContactEnumMetadata contactEmailType = {
	TypeEnum, EnumTypeEmail, makeRange(contactEmailTypeValues),
	CONTACTS_EMAIL_TYPE_HOME, CONTACTS_EMAIL_TYPE_CUSTOM
};
const ContactFieldMetadata contactEmailFields[] = {
	{ FieldEmail, _contacts_email.email, &contactEmailText }
};
const ContactTypedObjectMetadata contactEmail = {
	TypeObject, ObjectTypeEmail, _contacts_email._uri, makeRange(contactEmailFields),
	{ FieldEmail, _contacts_email.type,  UPCAST(&contactEmailType) },
	{ FieldEmail, _contacts_email.label, &contactRegularText }
};

/****************************** Address Object ********************************/
const int contactAddrTypeValues[] = {
	CONTACTS_ADDRESS_TYPE_HOME,
	CONTACTS_ADDRESS_TYPE_WORK,
	CONTACTS_ADDRESS_TYPE_OTHER,
	CONTACTS_ADDRESS_TYPE_CUSTOM
};
const ContactEnumMetadata contactAddrType = {
	TypeEnum, EnumTypeAddress, makeRange(contactAddrTypeValues),
	CONTACTS_ADDRESS_TYPE_HOME, CONTACTS_ADDRESS_TYPE_CUSTOM
};
const ContactFieldMetadata contactAddrFields[] = {
	{ FieldAddress, _contacts_address.street, &contactRegularText }
};
const ContactTypedObjectMetadata contactAddr = {
	TypeObject, ObjectTypeAddress, _contacts_address._uri, makeRange(contactAddrFields),
	{ FieldAddress, _contacts_address.type,  UPCAST(&contactAddrType) },
	{ FieldAddress, _contacts_address.label, &contactRegularText },
};

/******************************* Event Object *********************************/
const int contactEventTypeValues[] = {
	CONTACTS_EVENT_TYPE_BIRTH,
	CONTACTS_EVENT_TYPE_ANNIVERSARY,
	CONTACTS_EVENT_TYPE_OTHER,
	CONTACTS_EVENT_TYPE_CUSTOM
};
const ContactEnumMetadata contactEventType = {
	TypeEnum, EnumTypeEvent, makeRange(contactEventTypeValues),
	CONTACTS_EVENT_TYPE_BIRTH, CONTACTS_EVENT_TYPE_CUSTOM
};
const ContactFieldMetadata contactEventFields[] = {
	{ FieldEvent, _contacts_event.date, &contactDate }
};
const ContactTypedObjectMetadata contactEvent = {
	TypeObject, ObjectTypeEvent, _contacts_event._uri, makeRange(contactEventFields),
	{ FieldEvent, _contacts_event.type,  UPCAST(&contactEventType) },
	{ FieldEvent, _contacts_event.label, &contactRegularText }
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
	TypeEnum, EnumTypeRelationship, makeRange(contactRelTypeValues),
	CONTACTS_RELATIONSHIP_TYPE_ASSISTANT, CONTACTS_RELATIONSHIP_TYPE_CUSTOM
};
const ContactFieldMetadata contactRelFields[] = {
	{ FieldRelationship, _contacts_relationship.name, &contactRegularText }
};
const ContactTypedObjectMetadata contactRel = {
	TypeObject, ObjectTypeRelationship, _contacts_relationship._uri, makeRange(contactRelFields),
	{ FieldRelationship, _contacts_relationship.type,  UPCAST(&contactRelType) },
	{ FieldRelationship, _contacts_relationship.label, &contactRegularText }
};

/******************************** Url Object **********************************/
const int contactUrlTypeValues[] = {
	CONTACTS_URL_TYPE_HOME,
	CONTACTS_URL_TYPE_WORK,
	CONTACTS_URL_TYPE_OTHER,
	CONTACTS_URL_TYPE_CUSTOM
};
const ContactEnumMetadata contactUrlType = {
	TypeEnum, EnumTypeUrl, makeRange(contactUrlTypeValues),
	CONTACTS_URL_TYPE_HOME, CONTACTS_URL_TYPE_CUSTOM
};
const ContactFieldMetadata contactUrlFields[] = {
	{ FieldUrl, _contacts_url.url,  &contactUrlText }
};
const ContactTypedObjectMetadata contactUrl = {
	TypeObject, ObjectTypeUrl, _contacts_url._uri, makeRange(contactUrlFields),
	{ FieldUrl, _contacts_url.type, UPCAST(&contactUrlType) },
	{ FieldUrl, _contacts_url.type, &contactRegularText }
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
	TypeEnum, EnumTypeMessenger, makeRange(contactMsgTypeValues),
	CONTACTS_MESSENGER_TYPE_AIM, CONTACTS_MESSENGER_TYPE_CUSTOM
};
const ContactFieldMetadata contactMsgFields[] = {
	{ FieldMessenger, _contacts_messenger.im_id, &contactRegularText }
};
const ContactTypedObjectMetadata contactMsg = {
	TypeObject, ObjectTypeMessenger, _contacts_messenger._uri, makeRange(contactMsgFields),
	{ FieldMessenger, _contacts_messenger.type,  UPCAST(&contactMsgType) },
	{ FieldMessenger, _contacts_messenger.label, &contactRegularText }
};

/******************************* Array Fields *********************************/
const ContactArrayMetadata contactNumbers = {
	TypeArray, ObjectTypeNumber,
	FieldNumber, _contacts_contact.number, UPCAST(&contactNumber)
};
const ContactArrayMetadata contactEmails = {
	TypeArray, ObjectTypeEmail,
	FieldEmail, _contacts_contact.email, UPCAST(&contactEmail)
};
const ContactArrayMetadata contactAddrs = {
	TypeArray, ObjectTypeAddress,
	FieldAddress, _contacts_contact.address, UPCAST(&contactAddr)
};
const ContactArrayMetadata contactEvents = {
	TypeArray, ObjectTypeEvent,
	FieldEvent, _contacts_contact.event, UPCAST(&contactEvent)
};
const ContactArrayMetadata contactRels = {
	TypeArray, ObjectTypeRelationship,
	FieldRelationship, _contacts_contact.relationship, UPCAST(&contactRel)
};
const ContactArrayMetadata contactUrls = {
	TypeArray, ObjectTypeUrl,
	FieldUrl, _contacts_contact.url, UPCAST(&contactUrl)
};
const ContactArrayMetadata contactMsgs = {
	TypeArray, ObjectTypeMessenger,
	FieldMessenger, _contacts_contact.messenger, UPCAST(&contactMsg)
};

/****************************** Contact Object ********************************/
const ContactFieldMetadata contactFields[] = {
	{ FieldImage,        _contacts_contact.image,        UPCAST(&contactImage) },
	{ FieldName,         _contacts_contact.name,         UPCAST(&contactName) },
	{ FieldNumber,       _contacts_contact.number,       UPCAST(&contactNumbers) },
	{ FieldEmail,        _contacts_contact.email,        UPCAST(&contactEmails) },
	{ FieldNote,         _contacts_contact.note,         UPCAST(&contactNote) },
	{ FieldAddress,      _contacts_contact.address,      UPCAST(&contactAddrs) },
	{ FieldEvent,        _contacts_contact.event,        UPCAST(&contactEvents) },
	{ FieldRelationship, _contacts_contact.relationship, UPCAST(&contactRels) },
	{ FieldUrl,          _contacts_contact.url,          UPCAST(&contactUrls) },
	{ FieldMessenger,    _contacts_contact.messenger,    UPCAST(&contactMsgs) },
	{ FieldNickname,     _contacts_contact.nickname,     UPCAST(&contactNick) },
	{ FieldCompany,      _contacts_contact.company,      UPCAST(&contactCompany) },
	{ FieldPhoneticName, _contacts_contact.name,         UPCAST(&contactPhName) }
};
const ContactObjectMetadata contactObject = {
	TypeObject, ObjectTypeContact, _contacts_contact._uri, makeRange(contactFields)
};
const ContactFieldMetadata contact = { FieldRoot, 0, UPCAST(&contactObject) };

/****************************** My Profile Object ********************************/
const ContactFieldMetadata myProfileFields[] = {
	{ FieldImage,        _contacts_my_profile.image,        UPCAST(&contactImage) },
	{ FieldName,         _contacts_my_profile.name,         UPCAST(&contactName) },
	{ FieldNumber,       _contacts_my_profile.number,       UPCAST(&contactNumbers) },
	{ FieldEmail,        _contacts_my_profile.email,        UPCAST(&contactEmails) },
	{ FieldNote,         _contacts_my_profile.note,         UPCAST(&contactNote) },
	{ FieldAddress,      _contacts_my_profile.address,      UPCAST(&contactAddrs) },
	{ FieldEvent,        _contacts_my_profile.event,        UPCAST(&contactEvents) },
	{ FieldRelationship, _contacts_my_profile.relationship, UPCAST(&contactRels) },
	{ FieldUrl,          _contacts_my_profile.url,          UPCAST(&contactUrls) },
	{ FieldMessenger,    _contacts_my_profile.messenger,    UPCAST(&contactMsgs) },
	{ FieldNickname,     _contacts_my_profile.nickname,     UPCAST(&contactNick) },
	{ FieldCompany,      _contacts_my_profile.company,      UPCAST(&contactCompany) },
	{ FieldPhoneticName, _contacts_my_profile.name,         UPCAST(&contactPhName) }
};
const ContactObjectMetadata myProfileObject = {
	TypeObject, ObjectTypeMyProfile, _contacts_my_profile._uri, makeRange(myProfileFields)
};
const ContactFieldMetadata myProfile = { FieldRoot, 0, UPCAST(&myProfileObject) };

} /* Anonymous namespace */


const ContactFieldMetadata *Model::getContactMetadata()
{
	return &contact;
}

const ContactFieldMetadata *Model::getMyProfileMetadata()
{
	return &myProfile;
}
