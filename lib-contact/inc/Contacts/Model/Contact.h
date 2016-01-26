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

#ifndef CONTACTS_MODEL_CONTACT_H
#define CONTACTS_MODEL_CONTACT_H

#include "Contacts/Model/ContactObject.h"

namespace Contacts
{
	namespace Model
	{
		/**
		 * @brief Contact record wrapper for uniform handling of new or existing contact.
		 */
		class Contact : public ContactObject
		{
		public:
			/**
			 * @brief Create contact object.
			 * @param[in]   type    Contact object type (ObjectTypeContact or ObjectTypeMyProfile)
			 */
			Contact(ContactObjectType type);
			Contact(const Contact &that) = delete;
			virtual ~Contact() override;

			/**
			 * @brief Initialize contact object.
			 * @param[in]   id      Database record ID or 0 to create new contact
			 * @return Contacts API error code.
			 */
			int initialize(int recordId);

			/**
			 * @return Whether the contact is new and is not stored in database yet.
			 */
			bool isNew() const;

			/**
			 * @brief Save contact to the database.
			 * @return Contacts API error code.
			 */
			int save();

			/**
			 * @brief Remove contact from the database.
			 */
			int remove();

			Contact &operator=(const Contact &that) = delete;

		private:
			bool m_IsNew;
		};
	}
}

#endif /* CONTACTS_MODEL_CONTACT_H */
