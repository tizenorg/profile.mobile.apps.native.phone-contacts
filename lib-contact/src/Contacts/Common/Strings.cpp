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
#include "Utils/Range.h"

using namespace Contacts;
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

	const char *numberValueNames[] = {
		"IDS_PB_OPT_MOBILE",
		"IDS_PB_OPT_HOME_ABB",
		"IDS_PB_OPT_WORK",
		"IDS_PB_OPT_MAIN",
		"IDS_PB_OPT_FAX_HWORK_ABB",
		"IDS_PB_OPT_FAX_HHOME_ABB",
		"IDS_PB_OPT_PAGER",
		"IDS_PB_OPT_OTHER",
		"IDS_PB_OPT_CUSTOM"
	};

	const char *emailValueNames[] = {
		"IDS_PB_OPT_HOME_ABB",
		"IDS_PB_OPT_WORK",
		"IDS_PB_OPT_OTHER",
		"IDS_PB_OPT_CUSTOM"
	};

	const char *addressValueNames[] = {
		"IDS_PB_OPT_HOME_ABB",
		"IDS_PB_OPT_WORK",
		"IDS_PB_OPT_OTHER",
		"IDS_PB_OPT_CUSTOM"
	};

	const char *eventValueNames[] = {
		"IDS_PB_OPT_BIRTHDAY",
		"IDS_PB_OPT_ANNIVERSARY_ABB",
		"IDS_PB_OPT_OTHER",
		"IDS_PB_OPT_CUSTOM"
	};

	const char *relValueNames[] = {
		"IDS_PB_OPT_ASSISTANT_M_CONTACTS",
		"IDS_PB_OPT_BROTHER",
		"IDS_PB_OPT_CHILD",
		"IDS_PB_OPT_DOMESTIC_PARTNER_ABB2",
		"IDS_PB_OPT_FATHER",
		"IDS_PB_OPT_FRIEND",
		"IDS_PB_OPT_MANAGER",
		"IDS_PB_OPT_MOTHER",
		"IDS_PB_OPT_PARENT",
		"IDS_PB_OPT_PARTNER",
		"IDS_PB_OPT_REFERRED_BY_ABB",
		"IDS_PB_OPT_RELATIVE",
		"IDS_PB_OPT_SISTER",
		"IDS_PB_OPT_SPOUSE",
		"IDS_PB_OPT_CUSTOM"
	};

	const char *urlValueNames[] = {
		"IDS_PB_OPT_HOME_ABB",
		"IDS_PB_OPT_WORK",
		"IDS_PB_OPT_OTHER",
		"IDS_PB_OPT_CUSTOM"
	};

	const char *msgValueNames[] = {
		"IDS_PB_OPT_AIM",
		"IDS_PB_OPT_YAHOO_E",
		"IDS_PB_BODY_SKYPE",
		"IDS_PB_OPT_QQ",
		"IDS_PB_OPT_HANGOUTS",
		"IDS_PB_OPT_ICQ",
		"IDS_PB_OPT_JABBER",
		"IDS_PB_OPT_CUSTOM"
	};

	Utils::Range<const char **> enumValueNames[] = {
		/* [EnumTypeNumber]       = */ numberValueNames,
		/* [EnumTypeEmail]        = */ emailValueNames,
		/* [EnumTypeAddress]      = */ addressValueNames,
		/* [EnumTypeEvent]        = */ eventValueNames,
		/* [EnumTypeRelationship] = */ relValueNames,
		/* [EnumTypeUrl]          = */ urlValueNames,
		/* [EnumTypeMessenger]    = */ msgValueNames
	};
}

const char *Common::getContactFieldName(Model::ContactFieldId fieldId)
{
	return Utils::at(fieldNames, fieldId);
}

const char *Common::getContactChildFieldName(unsigned fieldId)
{
	return Utils::at(childFieldNames, fieldId);
}

Utils::Range<const char **> Common::getContactEnumValueNames(Model::ContactEnumType type)
{
	return Utils::at(enumValueNames, type);
}
