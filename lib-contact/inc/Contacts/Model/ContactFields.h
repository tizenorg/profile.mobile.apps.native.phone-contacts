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

#ifndef CONTACTS_MODEL_CONTACT_FIELDS_H
#define CONTACTS_MODEL_CONTACT_FIELDS_H

namespace Contacts
{
	namespace Model
	{
		/**
		 * @brief Fields of object of ObjectTypeContact or ObjectTypeMyProfile subtype
		 */
		enum ContactFieldId
		{
			FieldRoot, /**< Root object field ID (i.e Contact or My Profile) */
			FieldBegin,
			FieldImage = FieldBegin,
			FieldName,
			FieldPhoneticName,
			FieldCompany,
			FieldNumber,
			FieldEmail,
			FieldAddress,
			FieldUrl,
			FieldMessenger,
			FieldEvent,
			FieldNote,
			FieldNickname,
			FieldRelationship,
			FieldEnd
		};

		/**
		 * @brief Fields of object of ObjectTypeCompany subtype
		 */
		enum ContactCompanyFieldId
		{
			CompanyFieldBegin = FieldEnd,
			CompanyFieldName = CompanyFieldBegin,
			CompanyFieldJobTitle,
			CompanyFieldEnd
		};

		/**
		 * @brief Fields of object of ObjectTypeName subtype
		 */
		enum ContactNameFieldId
		{
			NameFieldBegin = CompanyFieldEnd,
			NameFieldTitle = NameFieldBegin,
			NameFieldFirst,
			NameFieldMiddle,
			NameFieldLast,
			NameFieldSuffix,
			NameFieldEnd
		};

		/**
		 * @brief Fields of object of ObjectTypePhoneticName subtype
		 */
		enum ContactPhoneticNameFieldId
		{
			PhoneticNameFieldBegin = NameFieldEnd,
			PhoneticNameFieldFirst = PhoneticNameFieldBegin,
			PhoneticNameFieldMiddle,
			PhoneticNameFieldLast,
			PhoneticNameFieldEnd
		};
	}
}

#endif /* CONTACTS_MODEL_CONTACT_FIELDS_H */
