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

/******************************** Bool type ***********************************/
const ContactTypeMetadata contactBool = { TypeBool, 0 };

/******************************** Text types **********************************/
const ContactTypeMetadata contactRegularText = { TypeText, TextTypeRegular };
const ContactTypeMetadata contactNumberText = { TypeText, TextTypeNumber };
const ContactTypeMetadata contactEmailText = { TypeText, TextTypeEmail };
const ContactTypeMetadata contactUrlText = { TypeText, TextTypeUrl };

/******************************** Date type ***********************************/
const ContactTypeMetadata contactDate = { TypeDate, 0 };

/****************************** Company Object ********************************/
const ContactFieldMetadata contactCompanyFields[] = {
	{ CompanyFieldName,     _contacts_company.name,      true,  &contactRegularText },
	{ CompanyFieldJobTitle, _contacts_company.job_title, false, &contactRegularText }
};
const ContactObjectMetadata contactCompany = {
	TypeObject, ObjectTypeCompany,
	_contacts_company._uri, _contacts_company.id, makeRange(contactCompanyFields)
};

/******************************* Name Object **********************************/
const ContactFieldMetadata contactNameFields[] = {
	{ NameFieldTitle,  _contacts_name.prefix,   false, &contactRegularText },
	{ NameFieldFirst,  _contacts_name.first,    true,  &contactRegularText },
	{ NameFieldMiddle, _contacts_name.addition, true,  &contactRegularText },
	{ NameFieldLast,   _contacts_name.last,     true,  &contactRegularText },
	{ NameFieldSuffix, _contacts_name.suffix,   false, &contactRegularText }
};
const ContactObjectMetadata contactName = {
	TypeObject, ObjectTypeName,
	_contacts_name._uri, _contacts_name.id, makeRange(contactNameFields)
};

/*************************** Phonetic Name Object *****************************/
const ContactFieldMetadata contactPhNameFields[] = {
	{ PhoneticNameFieldFirst,  _contacts_name.phonetic_first,  false, &contactRegularText },
	{ PhoneticNameFieldMiddle, _contacts_name.phonetic_middle, false, &contactRegularText },
	{ PhoneticNameFieldLast,   _contacts_name.phonetic_last,   false, &contactRegularText }
};
const ContactObjectMetadata contactPhName = {
	TypeObject, ObjectTypePhoneticName,
	_contacts_name._uri, _contacts_name.id, makeRange(contactPhNameFields)
};

/******************************** Note Object *********************************/
const ContactFieldMetadata contactNoteFields[] = {
	{ FieldNote, _contacts_note.note, false, &contactRegularText },
};
const ContactObjectMetadata contactNote = {
	TypeObject, ObjectTypeNote,
	_contacts_note._uri, _contacts_note.id, makeRange(contactNoteFields)
};

/***************************** Nickname Object ********************************/
const ContactFieldMetadata contactNickFields[] = {
	{ FieldNickname, _contacts_nickname.name, true, &contactRegularText },
};
const ContactObjectMetadata contactNick = {
	TypeObject, ObjectTypeNickname,
	_contacts_nickname._uri, _contacts_nickname.id, makeRange(contactNickFields)
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
	{ FieldImage, _contacts_image.path, false, &contactRegularText }
};
const ContactTypedObjectMetadata contactImage = {
	TypeObject, ObjectTypeImage,
	_contacts_image._uri, _contacts_image.id, makeRange(contactImageFields),
	{ FieldImage, _contacts_image.type,  false, UPCAST(&contactImageType) },
	{ FieldImage, _contacts_image.label, false, &contactRegularText }
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
	{ FieldNumber, _contacts_number.number, true, &contactNumberText }
};
const ContactTypedObjectMetadata contactNumber = {
	TypeObject, ObjectTypeNumber,
	_contacts_number._uri, _contacts_number.id, makeRange(contactNumberFields),
	{ FieldNumber, _contacts_number.type,  false, UPCAST(&contactNumberType) },
	{ FieldNumber, _contacts_number.label, false, &contactRegularText }
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
	{ FieldEmail, _contacts_email.email, true, &contactEmailText }
};
const ContactTypedObjectMetadata contactEmail = {
	TypeObject, ObjectTypeEmail,
	_contacts_email._uri, _contacts_email.id, makeRange(contactEmailFields),
	{ FieldEmail, _contacts_email.type,  false, UPCAST(&contactEmailType) },
	{ FieldEmail, _contacts_email.label, false, &contactRegularText }
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
	{ FieldAddress, _contacts_address.street, false, &contactRegularText }
};
const ContactTypedObjectMetadata contactAddr = {
	TypeObject, ObjectTypeAddress,
	_contacts_address._uri, _contacts_address.id, makeRange(contactAddrFields),
	{ FieldAddress, _contacts_address.type,  false, UPCAST(&contactAddrType) },
	{ FieldAddress, _contacts_address.label, false, &contactRegularText },
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
	{ FieldEvent, _contacts_event.date, false, &contactDate }
};
const ContactTypedObjectMetadata contactEvent = {
	TypeObject, ObjectTypeEvent,
	_contacts_event._uri, _contacts_event.id, makeRange(contactEventFields),
	{ FieldEvent, _contacts_event.type,  false, UPCAST(&contactEventType) },
	{ FieldEvent, _contacts_event.label, false, &contactRegularText }
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
	{ FieldRelationship, _contacts_relationship.name, false, &contactRegularText }
};
const ContactTypedObjectMetadata contactRel = {
	TypeObject, ObjectTypeRelationship,
	_contacts_relationship._uri, _contacts_relationship.id, makeRange(contactRelFields),
	{ FieldRelationship, _contacts_relationship.type,  false, UPCAST(&contactRelType) },
	{ FieldRelationship, _contacts_relationship.label, false, &contactRegularText }
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
	{ FieldUrl, _contacts_url.url, false, &contactUrlText }
};
const ContactTypedObjectMetadata contactUrl = {
	TypeObject, ObjectTypeUrl,
	_contacts_url._uri, _contacts_url.id, makeRange(contactUrlFields),
	{ FieldUrl, _contacts_url.type, false, UPCAST(&contactUrlType) },
	{ FieldUrl, _contacts_url.type, false, &contactRegularText }
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
	{ FieldMessenger, _contacts_messenger.im_id, false, &contactRegularText }
};
const ContactTypedObjectMetadata contactMsg = {
	TypeObject, ObjectTypeMessenger,
	_contacts_messenger._uri, _contacts_messenger.id, makeRange(contactMsgFields),
	{ FieldMessenger, _contacts_messenger.type,  false, UPCAST(&contactMsgType) },
	{ FieldMessenger, _contacts_messenger.label, false, &contactRegularText }
};

/******************************* Array Fields *********************************/
const ContactArrayMetadata contactNumbers = {
	TypeArray, ObjectTypeNumber,
	{ FieldNumber, _contacts_contact.number, true, UPCAST(&contactNumber) }
};
const ContactArrayMetadata contactEmails = {
	TypeArray, ObjectTypeEmail,
	{ FieldEmail, _contacts_contact.email, true, UPCAST(&contactEmail) }
};
const ContactArrayMetadata contactAddrs = {
	TypeArray, ObjectTypeAddress,
	{ FieldAddress, _contacts_contact.address, false, UPCAST(&contactAddr) }
};
const ContactArrayMetadata contactEvents = {
	TypeArray, ObjectTypeEvent,
	{ FieldEvent, _contacts_contact.event, false, UPCAST(&contactEvent) }
};
const ContactArrayMetadata contactRels = {
	TypeArray, ObjectTypeRelationship,
	{ FieldRelationship, _contacts_contact.relationship, false, UPCAST(&contactRel) }
};
const ContactArrayMetadata contactUrls = {
	TypeArray, ObjectTypeUrl,
	{ FieldUrl, _contacts_contact.url, false, UPCAST(&contactUrl) }
};
const ContactArrayMetadata contactMsgs = {
	TypeArray, ObjectTypeMessenger,
	{ FieldMessenger, _contacts_contact.messenger, false, UPCAST(&contactMsg) }
};

/****************************** Contact Object ********************************/
const ContactFieldMetadata contactFields[] = {
	{ FieldFavorite,     _contacts_contact.is_favorite,  false, UPCAST(&contactBool) },
	{ FieldImage,        _contacts_contact.image,        false, UPCAST(&contactImage) },
	{ FieldName,         _contacts_contact.name,         true,  UPCAST(&contactName) },
	{ FieldPhoneticName, _contacts_contact.name,         false, UPCAST(&contactPhName) },
	{ FieldCompany,      _contacts_contact.company,      true,  UPCAST(&contactCompany) },
	{ FieldNumber,       _contacts_contact.number,       true,  UPCAST(&contactNumbers) },
	{ FieldEmail,        _contacts_contact.email,        true,  UPCAST(&contactEmails) },
	{ FieldAddress,      _contacts_contact.address,      false, UPCAST(&contactAddrs) },
	{ FieldUrl,          _contacts_contact.url,          false, UPCAST(&contactUrls) },
	{ FieldMessenger,    _contacts_contact.messenger,    false, UPCAST(&contactMsgs) },
	{ FieldEvent,        _contacts_contact.event,        false, UPCAST(&contactEvents) },
	{ FieldNote,         _contacts_contact.note,         false, UPCAST(&contactNote) },
	{ FieldNickname,     _contacts_contact.nickname,     true,  UPCAST(&contactNick) },
	{ FieldRelationship, _contacts_contact.relationship, false, UPCAST(&contactRels) }
};
const ContactObjectMetadata contactObject = {
	TypeObject, ObjectTypeContact,
	_contacts_contact._uri, _contacts_contact.id, makeRange(contactFields)
};
const ContactFieldMetadata contact = { 0, 0, true, UPCAST(&contactObject) };

/****************************** My Profile Object *****************************/
const ContactFieldMetadata myProfileFields[] = {
	{ FieldImage,        _contacts_my_profile.image,        false, UPCAST(&contactImage) },
	{ FieldName,         _contacts_my_profile.name,         true,  UPCAST(&contactName) },
	{ FieldPhoneticName, _contacts_my_profile.name,         false, UPCAST(&contactPhName) },
	{ FieldCompany,      _contacts_my_profile.company,      true,  UPCAST(&contactCompany) },
	{ FieldNumber,       _contacts_my_profile.number,       true,  UPCAST(&contactNumbers) },
	{ FieldEmail,        _contacts_my_profile.email,        true,  UPCAST(&contactEmails) },
	{ FieldAddress,      _contacts_my_profile.address,      false, UPCAST(&contactAddrs) },
	{ FieldUrl,          _contacts_my_profile.url,          false, UPCAST(&contactUrls) },
	{ FieldMessenger,    _contacts_my_profile.messenger,    false, UPCAST(&contactMsgs) },
	{ FieldEvent,        _contacts_my_profile.event,        false, UPCAST(&contactEvents) },
	{ FieldNote,         _contacts_my_profile.note,         false, UPCAST(&contactNote) },
	{ FieldNickname,     _contacts_my_profile.nickname,     true,  UPCAST(&contactNick) },
	{ FieldRelationship, _contacts_my_profile.relationship, false, UPCAST(&contactRels) }
};
const ContactObjectMetadata myProfileObject = {
	TypeObject, ObjectTypeMyProfile,
	_contacts_my_profile._uri, _contacts_my_profile.id, makeRange(myProfileFields)
};
const ContactFieldMetadata myProfile = { 0, 0, true, UPCAST(&myProfileObject) };

} /* Anonymous namespace */


const ContactFieldMetadata *Model::getContactMetadata(ContactObjectType type)
{
	if (type == ObjectTypeContact) {
		return &contact;
	} else if (type == ObjectTypeMyProfile) {
		return &myProfile;
	}

	return nullptr;
}
