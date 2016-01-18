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

#ifndef CONTACTS_MODEL_CONTACT_TYPED_OBJECT_H
#define CONTACTS_MODEL_CONTACT_TYPED_OBJECT_H

#include "Contacts/Model/ContactObject.h"
#include "Contacts/Model/ContactEnumField.h"
#include "Contacts/Model/ContactTextField.h"

namespace Contacts
{
	namespace Model
	{
		class ContactTypedObjectMetadata;

		/**
		 * @brief Adapter for an object that has a "type" represented by
		 *        TypeEnum and TypeText fields.
		 */
		class ContactTypedObject : public ContactObject
		{
		public:
			using ContactObject::ContactObject;

			/**
			 * @see ContactField::initialize()
			 */
			virtual void initialize() override;

			/**
			 * @see ContactField::reset()
			 */
			virtual void reset() override;

			/**
			 * @return Field that represents object's type.
			 */
			ContactEnumField &getTypeField() const;

			/**
			 * @return Field that stores object's custom type label.
			 */
			ContactTextField &getLabelField() const;

		private:
			const ContactTypedObjectMetadata &getTypedObjectMetadata() const;

			ContactFieldPtr m_TypeField;
			ContactFieldPtr m_LabelField;
		};
	}
}

#endif /* CONTACTS_MODEL_CONTACT_TYPED_OBJECT_H */
