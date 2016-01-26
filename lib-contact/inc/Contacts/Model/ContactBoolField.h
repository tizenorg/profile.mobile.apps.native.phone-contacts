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

#ifndef CONTACTS_MODEL_CONTACT_BOOL_FIELD_H
#define CONTACTS_MODEL_CONTACT_BOOL_FIELD_H

#include "Contacts/Model/ContactField.h"

namespace Contacts
{
	namespace Model
	{
		/**
		 * @brief Adaptor for property containing boolean value
		 *
		 * @see TypeBool
		 */
		class ContactBoolField : public ContactField
		{
		public:
			using ContactField::ContactField;

			/**
			 * @return Field value.
			 */
			bool getValue() const;

			/**
			 * @brief Set field value.
			 */
			void setValue(bool value);
		};
	}
}

#endif /* CONTACTS_MODEL_CONTACT_BOOL_FIELD_H */
