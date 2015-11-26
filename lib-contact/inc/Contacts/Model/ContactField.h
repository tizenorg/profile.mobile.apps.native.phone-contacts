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
#ifndef CONTACTS_MODEL_CONTACT_FIELD_H
#define CONTACTS_MODEL_CONTACT_FIELD_H

#include <contacts.h>
#include <memory>

#include "Contacts/Model/ContactFields.h"
#include "Contacts/Model/ContactTypes.h"

namespace Contacts
{
	namespace Model
	{
		class ContactFieldMetadata;

		/**
		 * @brief Polymorphic adapter for contacts_record_h property.
		 * @remark Adapter object is not responsible for record's lifetime
		 *         and can be freely copied and destroyed.
		 */
		class ContactField
		{
		public:
			/**
			 * @brief Create contact field.
			 * @param[in]   record      Record containing the field
			 * @param[in]   fieldId     Field ID
			 * @param[in]   metadata    Field metadata
			 */
			ContactField(contacts_record_h record, unsigned fieldId,
					const ContactFieldMetadata *metadata);
			virtual ~ContactField() { }

			/**
			 * @brief Initialize field with default values.
			 */
			virtual void initialize() { }

			/**
			 * @return Whether field has no value.
			 */
			virtual bool isEmpty() const { return false; }

			/**
			 * @return Field ID.
			 *
			 * @see ContactFieldId
			 * @see ContactCompanyFieldId
			 * @see ContactNameFieldId
			 * @see ContactPhoneticNameFieldId
			 */
			unsigned getId() const;

			/**
			 * @return Field data type.
			 *
			 * @see ContactFieldType
			 */
			ContactFieldType getType() const;

			/**
			 * @return Field data subtype.
			 *
			 * @see ContactFieldObjectType
			 * @see ContactFieldEnumType
			 * @see ContactFieldTextType
			 */
			unsigned getSubType() const;

			/**
			 * @return Record containing the field.
			 */
			contacts_record_h getRecord() const;

			/**
			 * @return Contacts API property ID corresponding to this field.
			 */
			unsigned getPropertyId() const;

			/**
			 * @brief Get record property ID by field ID.
			 * @param[in]   fieldId     Field ID
			 * @return Record property ID.
			 */
			static unsigned getPropertyId(unsigned fieldId, ContactFieldType fieldType);

			/**
			 * @return Contacts API view URI which represents this fields subtype.
			 */
			const char *getUri() const;

			/**
			 * @brief Get Contacts API view URI which represents the subtype.
			 * @param[in]   fieldSubType    Field subtype.
			 * @return View URI.
			 *
			 * @see ContactFieldObjectType
			 */
			static const char *getUri(unsigned fieldSubType);

		protected:
			const ContactFieldMetadata *getMetadata() const;

		private:
			contacts_record_h m_Record;
			const char *m_Uri;
			unsigned m_PropId;

			unsigned m_FieldId;
			const ContactFieldMetadata *m_Metadata;
		};

		typedef std::unique_ptr<ContactField> ContactFieldPtr;
	}
}

#endif /* CONTACTS_MODEL_CONTACT_FIELD_H */
