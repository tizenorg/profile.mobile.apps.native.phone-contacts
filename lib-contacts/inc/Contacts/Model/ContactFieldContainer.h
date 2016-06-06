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

#ifndef CONTACTS_MODEL_CONTACT_FIELD_CONTAINER_H
#define CONTACTS_MODEL_CONTACT_FIELD_CONTAINER_H

#include "Contacts/Model/ContactField.h"
#include "Contacts/Model/ContactFieldIterator.h"

namespace Contacts
{
	namespace Model
	{
		/**
		 * @brief Base class for a field containing other fields.
		 */
		class ContactFieldContainer : public ContactField
		{
		public:
			using ContactField::ContactField;

			/**
			 * @see ContactField::reset()
			 */
			virtual void reset() override;

			/**
			 * @see ContactField::isEmpty()
			 */
			virtual bool isEmpty() const override;

			/**
			 * @see ContactField::isFilled()
			 */
			virtual bool isFilled() const override;

			/**
			 * @see ContactField::isChanged()
			 */
			virtual bool isChanged() const override;

			/**
			 * @return Begin iterator.
			 */
			ContactFieldIterator begin() const;

			/**
			 * @return End iterator.
			 */
			ContactFieldIterator end() const;

			/**
			 * @brief Get child field by index.
			 * @param[in]   index   Child field index
			 * @return Child field.
			 */
			template <typename FieldType>
			FieldType *getField(unsigned index) const;
			ContactField *getField(unsigned index) const;

			/**
			 * @return Child field count.
			 */
			size_t getFieldCount() const;

		protected:
			ContactField &addField(contacts_record_h record,
					const ContactFieldMetadata &metadata);
			void removeField(ContactField &field);

		private:
			friend ContactField;
			void onChildFilled(ContactField &field, bool isChildFilled);
			void onChildUpdated(ContactField &field, contacts_changed_e change);

			ContactFields m_Fields;
			size_t m_FilledCount = 0;
		};

		template <typename FieldType>
		FieldType *ContactFieldContainer::getField(unsigned index) const
		{
			return static_cast<FieldType *>(getField(index));
		}
	}
}

#endif /* CONTACTS_MODEL_CONTACT_FIELD_CONTAINER_H */
