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

#include "Contacts/Common/Strings.h"
#include <contacts.h>

using namespace Contacts;
using namespace Contacts::Common;
using namespace Contacts::Model;

namespace
{
	const char *fieldNames[] = {
		/* [FieldFavorite]     = */ nullptr,
		/* [FieldDisplayName]  = */ nullptr,
		/* [FieldImage]        = */ "IDS_PB_BODY_PHOTO",
		/* [FieldName]         = */ "IDS_PB_BODY_NAME",
		/* [FieldPhoneticName] = */ "IDS_PB_BODY_PHONETIC_NAME_ABB",
		/* [FieldCompany]      = */ "IDS_PB_OPT_ORGANISATION_ABB",
		/* [FieldNumber]       = */ "IDS_PB_BUTTON_PHONE",
		/* [FieldEmail]        = */ "IDS_PB_BUTTON_EMAIL_ABB3",
		/* [FieldAddress]      = */ "IDS_PB_BODY_ADDRESS_ABB2",
		/* [FieldUrl]          = */ "IDS_PB_BODY_WEBSITE_ABB",
		/* [FieldMessenger]    = */ "IDS_PB_BODY_IM",
		/* [FieldEvent]        = */ "IDS_PB_MBODY_EVENT",
		/* [FieldNote]         = */ "IDS_PB_BUTTON_NOTES",
		/* [FieldNickname]     = */ "IDS_PB_BODY_NICKNAME_ABB",
		/* [FieldRelationship] = */ "IDS_PB_OPT_RELATIONSHIP_ABB2"
	};

	const char *childFieldNames[] = {
		/* [FieldFavorite]     = */ nullptr,
		/* [FieldDisplayName]  = */ nullptr,
		/* [FieldImage]        = */ "IDS_PB_BODY_IMAGE",
		/* [FieldName]         = */ "IDS_PB_BODY_NAME",
		/* [FieldPhoneticName] = */ "IDS_PB_BODY_PHONETIC_NAME_ABB",
		/* [FieldCompany]      = */ "IDS_PB_BODY_COMPANY_ABB",
		/* [FieldNumber]       = */ "IDS_PB_BODY_PHONE_NUMBER_ABB",
		/* [FieldEmail]        = */ "IDS_PB_BODY_EMAIL",
		/* [FieldAddress]      = */ "IDS_PB_BODY_ADDRESS_ABB2",
		/* [FieldUrl]          = */ "IDS_PB_BODY_WEBSITE_ABB",
		/* [FieldMessenger]    = */ "IDS_PB_BODY_IM",
		/* [FieldEvent]        = */ "IDS_PB_MBODY_EVENT",
		/* [FieldNote]         = */ "IDS_PB_BODY_NOTES",
		/* [FieldNickname]     = */ "IDS_PB_BODY_NICKNAME_ABB",
		/* [FieldRelationship] = */ "IDS_PB_BODY_NAME",

		/* [CompanyFieldName]     = */ "IDS_PB_BODY_COMPANY_ABB",
		/* [CompanyFieldJobTitle] = */ "IDS_PB_BODY_JOB_TITLE_ABB",

		/* [NameFieldTitle]  = */ "IDS_PB_BODY_TITLE_M_NAMEPREFIX",
		/* [NameFieldFirst]  = */ "IDS_PB_BODY_FIRST_NAME_ABB",
		/* [NameFieldMiddle] = */ "IDS_PB_BODY_MIDDLE_NAME_ABB",
		/* [NameFieldLast]   = */ "IDS_PB_BODY_LAST_NAME_ABB",
		/* [NameFieldSuffix] = */ "IDS_PB_BODY_NAME_SUFFIX_ABB",

		/* [PhoneticNameFieldFirst]  = */ "IDS_PB_BODY_PHONETIC_FIRST_NAME_ABB",
		/* [PhoneticNameFieldMiddle] = */ "IDS_PB_BODY_PHONETIC_MIDDLE_NAME_ABB",
		/* [PhoneticNameFieldLast]   = */ "IDS_PB_BODY_PHONETIC_LAST_NAME_ABB"
	};

	ContactEnumValueName numberValueNames[] = {
		{ CONTACTS_NUMBER_TYPE_CELL,                              "IDS_PB_OPT_MOBILE" },
		{ CONTACTS_NUMBER_TYPE_VOICE | CONTACTS_NUMBER_TYPE_HOME, "IDS_PB_OPT_HOME_ABB" },
		{ CONTACTS_NUMBER_TYPE_VOICE | CONTACTS_NUMBER_TYPE_WORK, "IDS_PB_OPT_WORK" },
		{ CONTACTS_NUMBER_TYPE_MAIN,                              "IDS_PB_OPT_MAIN" },
		{ CONTACTS_NUMBER_TYPE_FAX | CONTACTS_NUMBER_TYPE_WORK,   "IDS_PB_OPT_FAX_HWORK_ABB" },
		{ CONTACTS_NUMBER_TYPE_FAX | CONTACTS_NUMBER_TYPE_HOME,   "IDS_PB_OPT_FAX_HHOME_ABB" },
		{ CONTACTS_NUMBER_TYPE_PAGER,                             "IDS_PB_OPT_PAGER" },
		{ CONTACTS_NUMBER_TYPE_OTHER,                             "IDS_PB_OPT_OTHER" },
		{ CONTACTS_NUMBER_TYPE_CUSTOM,                            "IDS_PB_OPT_CUSTOM" }
	};

	ContactEnumValueName emailValueNames[] = {
		{ CONTACTS_EMAIL_TYPE_HOME,         "IDS_PB_OPT_HOME_ABB" },
		{ CONTACTS_EMAIL_TYPE_WORK,         "IDS_PB_OPT_WORK" },
		{ CONTACTS_EMAIL_TYPE_OTHER,        "IDS_PB_OPT_OTHER" },
		{ CONTACTS_EMAIL_TYPE_CUSTOM,       "IDS_PB_OPT_CUSTOM" }
	};

	ContactEnumValueName addressValueNames[] = {
		{ CONTACTS_ADDRESS_TYPE_HOME,       "IDS_PB_OPT_HOME_ABB" },
		{ CONTACTS_ADDRESS_TYPE_WORK,       "IDS_PB_OPT_WORK" },
		{ CONTACTS_ADDRESS_TYPE_OTHER,      "IDS_PB_OPT_OTHER" },
		{ CONTACTS_ADDRESS_TYPE_CUSTOM,     "IDS_PB_OPT_CUSTOM" }
	};

	ContactEnumValueName eventValueNames[] = {
		{ CONTACTS_EVENT_TYPE_BIRTH,        "IDS_PB_OPT_BIRTHDAY" },
		{ CONTACTS_EVENT_TYPE_ANNIVERSARY,  "IDS_PB_OPT_ANNIVERSARY_ABB" },
		{ CONTACTS_EVENT_TYPE_OTHER,        "IDS_PB_OPT_OTHER" },
		{ CONTACTS_EVENT_TYPE_CUSTOM,       "IDS_PB_OPT_CUSTOM" }
	};

	ContactEnumValueName relValueNames[] = {
		{ CONTACTS_RELATIONSHIP_TYPE_ASSISTANT,        "IDS_PB_OPT_ASSISTANT_M_CONTACTS" },
		{ CONTACTS_RELATIONSHIP_TYPE_BROTHER,          "IDS_PB_OPT_BROTHER" },
		{ CONTACTS_RELATIONSHIP_TYPE_CHILD,            "IDS_PB_OPT_CHILD" },
		{ CONTACTS_RELATIONSHIP_TYPE_DOMESTIC_PARTNER, "IDS_PB_OPT_DOMESTIC_PARTNER_ABB2" },
		{ CONTACTS_RELATIONSHIP_TYPE_FATHER,           "IDS_PB_OPT_FATHER" },
		{ CONTACTS_RELATIONSHIP_TYPE_FRIEND,           "IDS_PB_OPT_FRIEND" },
		{ CONTACTS_RELATIONSHIP_TYPE_MANAGER,          "IDS_PB_OPT_MANAGER" },
		{ CONTACTS_RELATIONSHIP_TYPE_MOTHER,           "IDS_PB_OPT_MOTHER" },
		{ CONTACTS_RELATIONSHIP_TYPE_PARENT,           "IDS_PB_OPT_PARENT" },
		{ CONTACTS_RELATIONSHIP_TYPE_PARTNER,          "IDS_PB_OPT_PARTNER" },
		{ CONTACTS_RELATIONSHIP_TYPE_REFERRED_BY,      "IDS_PB_OPT_REFERRED_BY_ABB" },
		{ CONTACTS_RELATIONSHIP_TYPE_RELATIVE,         "IDS_PB_OPT_RELATIVE" },
		{ CONTACTS_RELATIONSHIP_TYPE_SISTER,           "IDS_PB_OPT_SISTER" },
		{ CONTACTS_RELATIONSHIP_TYPE_SPOUSE,           "IDS_PB_OPT_SPOUSE" },
		{ CONTACTS_RELATIONSHIP_TYPE_CUSTOM,           "IDS_PB_OPT_CUSTOM" }
	};

	ContactEnumValueName urlValueNames[] = {
		{ CONTACTS_URL_TYPE_HOME,   "IDS_PB_OPT_HOME_ABB" },
		{ CONTACTS_URL_TYPE_WORK,   "IDS_PB_OPT_WORK" },
		{ CONTACTS_URL_TYPE_OTHER,  "IDS_PB_OPT_OTHER" },
		{ CONTACTS_URL_TYPE_CUSTOM, "IDS_PB_OPT_CUSTOM" }
	};

	ContactEnumValueName msgValueNames[] = {
		{ CONTACTS_MESSENGER_TYPE_AIM,    "IDS_PB_OPT_AIM" },
		{ CONTACTS_MESSENGER_TYPE_YAHOO,  "IDS_PB_OPT_YAHOO_E" },
		{ CONTACTS_MESSENGER_TYPE_SKYPE,  "IDS_PB_BODY_SKYPE" },
		{ CONTACTS_MESSENGER_TYPE_QQ,     "IDS_PB_OPT_QQ" },
		{ CONTACTS_MESSENGER_TYPE_GOOGLE, "IDS_PB_OPT_HANGOUTS" },
		{ CONTACTS_MESSENGER_TYPE_ICQ,    "IDS_PB_OPT_ICQ" },
		{ CONTACTS_MESSENGER_TYPE_JABBER, "IDS_PB_OPT_JABBER" },
		{ CONTACTS_MESSENGER_TYPE_CUSTOM, "IDS_PB_OPT_CUSTOM" }
	};

	Utils::Range<const ContactEnumValueName *> enumValueNames[] = {
		/* [EnumTypeNumber]       = */ numberValueNames,
		/* [EnumTypeEmail]        = */ emailValueNames,
		/* [EnumTypeAddress]      = */ addressValueNames,
		/* [EnumTypeEvent]        = */ eventValueNames,
		/* [EnumTypeRelationship] = */ relValueNames,
		/* [EnumTypeUrl]          = */ urlValueNames,
		/* [EnumTypeMessenger]    = */ msgValueNames
	};
}

const char *Common::getContactFieldName(ContactFieldId fieldId)
{
	return Utils::at(fieldNames, fieldId);
}

const char *Common::getContactChildFieldName(unsigned fieldId)
{
	return Utils::at(childFieldNames, fieldId);
}

Utils::Range<const ContactEnumValueName *> Common::getContactEnumValueNames(ContactEnumType type)
{
	return Utils::at(enumValueNames, type);
}

const char *Common::getContactEnumValueName(Model::ContactEnumType type, int value)
{
	auto pairs = getContactEnumValueNames(type);
	if (!pairs) {
		return nullptr;
	}

	for (auto &&pair : pairs) {
		if (pair.value == value) {
			return pair.name;
		}
	}

	return nullptr;
}
