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
#ifndef CONTACTS_MODEL_CONTACT_ARRAY_H
#define CONTACTS_MODEL_CONTACT_ARRAY_H

#include "Contacts/Model/ContactField.h"
#include "Contacts/Model/ContactIterator.h"

namespace Contacts
{
	namespace Model
	{
		class ContactArrayMetadata;

		/**
		 * @brief Adaptor for property containing multiple records.
		 */
		class ContactArray : public ContactField
		{
		public:
			using ContactField::ContactField;

			/**
			 * @see ContactField::isEmpty()
			 */
			virtual bool isEmpty() const override;

			/**
			 * @return Begin iterator.
			 */
			ContactArrayIterator begin() const;

			/**
			 * @return End iterator.
			 */
			ContactArrayIterator end() const;

			/**
			 * @brief Get child field by index.
			 * @param[in]   index   Child field index
			 * @return Child field.
			 */
			ContactFieldPtr getField(unsigned index) const;

			/**
			 * @brief Add new child field.
			 * @return New child field.
			 */
			ContactFieldPtr addField();

			/**
			 * @brief Remove child field and destroy the object.
			 * @param[in]   childField  Child field.
			 */
			void removeField(ContactField *field);

		private:
			const ContactArrayMetadata *getArrayMetadata() const;
		};
	}
}

#endif /* CONTACTS_MODEL_CONTACT_ARRAY_H */
