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
#ifndef CONTACTS_MODEL_CONTACT_FIELD_FACTORY_H
#define CONTACTS_MODEL_CONTACT_FIELD_FACTORY_H

#include "Contacts/Model/ContactObject.h"

namespace Contacts
{
	namespace Model
	{
		/**
		 * @brief Contact adaptor objects and fields factory
		 */
		class ContactFactory
		{
		public:
			/**
			 * @brief Create contact object adaptor for a record.
			 * @param[in]   record      _contacts_contact record
			 * @return Contact adaptor object.
			 */
			static ContactObject createContact(contacts_record_h record);

			/**
			 * @brief Create "My Profile" object adaptor for a record.
			 * @param[in]   record      _contacts_my_profile record
			 * @return Contact adaptor object.
			 */
			static ContactObject createMyProfile(contacts_record_h record);

			/**
			 * @brief Create record property adaptor.
			 * @param[in]   record      Record to which property belongs
			 * @param[in]   fieldId     Adaptor field ID
			 * @param[in]   metadata    Adaptor field metadata
			 * @return Adaptor field.
			 */
			static ContactFieldPtr createField(contacts_record_h record,
					unsigned fieldId, const ContactFieldMetadata *metadata);

		private:
			static contacts_record_h getObjectRecord(contacts_record_h record,
					unsigned fieldId, const ContactFieldMetadata *metadata);
		};
	}
}

#endif /* CONTACTS_MODEL_CONTACT_FIELD_FACTORY_H */
