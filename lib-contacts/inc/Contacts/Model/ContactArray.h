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

#include "Contacts/Model/ContactFieldContainer.h"

namespace Contacts
{
	namespace Model
	{
		struct ContactArrayMetadata;

		/**
		 * @brief Adaptor for property containing multiple records.
		 */
		class ContactArray : public ContactFieldContainer
		{
		public:
			using ContactFieldContainer::ContactFieldContainer;

			/**
			 * @see ContactField::reset()
			 */
			virtual void reset() override;

			/**
			 * @see ContactField::isChanged()
			 */
			virtual bool isChanged() const override;

			/**
			 * @brief Add new child field.
			 * @return New child field.
			 */
			ContactField &addField();

			/**
			 * @brief Remove child field and destroy the object.
			 * @param[in]   childField  Child field.
			 */
			void removeField(ContactField &field);

		protected:
			/**
			 * @return Array type metadata.
			 */
			const ContactArrayMetadata &getArrayMetadata() const;

			/**
			 * @see ContactField::onInitialize()
			 */
			virtual void onInitialize(contacts_record_h record) override;

			/**
			 * @see ContactField::onUpdate()
			 */
			virtual void onUpdate(contacts_record_h record) override;

		private:
			ContactField &addField(contacts_record_h record);

			size_t m_InitialCount;
		};
	}
}

#endif /* CONTACTS_MODEL_CONTACT_ARRAY_H */
