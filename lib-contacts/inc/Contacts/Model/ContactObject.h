/*
 * Copyright (c) 2015-2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef CONTACTS_MODEL_CONTACT_OBJECT_H
#define CONTACTS_MODEL_CONTACT_OBJECT_H

#include "Contacts/Model/ContactFieldContainer.h"

namespace Contacts
{
	namespace Model
	{
		struct ContactObjectMetadata;

		/**
		 * @brief Adapter for contacts_record_h object which may itself be
		 *        a field of another object.
		 */
		class ContactObject : public ContactFieldContainer
		{
		public:
			using ContactFieldContainer::ContactFieldContainer;

			/**
			 * @return Interfaces supported by the object.
			 */
			unsigned getInterfaces() const;

			/**
			 * @brief Get child field by id.
			 * @param[in]   id      Child field id
			 * @return Child field.
			 */
			template <typename FieldType>
			FieldType *getFieldById(unsigned id) const;
			ContactField *getFieldById(unsigned id) const;

			/**
			 * @return Object's record ID.
			 */
			int getRecordId() const;

			/**
			 * @brief Compare object with record by ID.
			 */
			bool operator==(contacts_record_h record) const;

		protected:
			/**
			 * @return Object type metadata.
			 */
			const ContactObjectMetadata &getObjectMetadata() const;

			/**
			 * @see ContactField::onInitialize()
			 */
			virtual void onInitialize(contacts_record_h record) override;

			/**
			 * @see ContactField::onUpdate()
			 */
			virtual void onUpdate(contacts_record_h record) override;

		private:
			contacts_record_h getChildRecord(contacts_record_h record,
					const ContactFieldMetadata &metadata);
		};

		template <typename FieldType>
		FieldType *ContactObject::getFieldById(unsigned id) const
		{
			return static_cast<FieldType *>(getFieldById(id));
		}
	}
}

#endif /* CONTACTS_MODEL_CONTACT_OBJECT_H */
