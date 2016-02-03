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
#include <functional>
#include <memory>
#include <vector>

#include "Contacts/Model/ContactFields.h"
#include "Contacts/Model/ContactTypes.h"

namespace Contacts
{
	namespace Model
	{
		class ContactFieldMetadata;
		class ContactFieldContainer;

		/**
		 * @brief Polymorphic adapter for contacts_record_h property.
		 * @remark Adapter object is not responsible for record's lifetime
		 *         and can be freely copied and destroyed.
		 */
		class ContactField
		{
		public:
			/**
			 * @brief Callback to be called when the field becomes filled or not filled.
			 * @param[in]   Whether the field is filled.
			 */
			typedef std::function<void(bool)> FillCallback;

			/**
			 * @brief Create contact field.
			 * @param[in]   parent      Parent field container
			 * @param[in]   metadata    Field metadata
			 */
			ContactField(ContactFieldContainer *parent,
					const ContactFieldMetadata &metadata);
			virtual ~ContactField() { }

			/**
			 * @brief Initialize the adapter with database record.
			 * @param[in]   record      Record containing the field
			 */
			void initialize(contacts_record_h record);

			/**
			 * @brief Reset field values to default.
			 */
			virtual void reset() { }

			/**
			 * @return Whether field has no value.
			 */
			virtual bool isEmpty() const { return false; }

			/**
			 * @return Whether the field is filled itself or one of it's child
			 *         required fields is filled.
			 */
			virtual bool isFilled() const { return !isEmpty(); }

			/**
			 * @return Whether the field value was changed.
			 */
			virtual bool isChanged() const { return false; }

			/**
			 * @return Whether the field determines that parent object is filled.
			 */
			bool isRequired() const;

			/**
			 * @brief Set field fill callback.
			 * @param[in]   callback    Called when field fill state is changed.
			 */
			void setFillCallback(FillCallback callback);

			/**
			 * @brief Cast field to derived type.
			 */
			template <typename FieldType>
			FieldType &cast();

			template <typename FieldType>
			const FieldType &cast() const;

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
			 * @see ContactObjectType
			 * @see ContactEnumType
			 * @see ContactTextType
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
			 * @return Parent container.
			 */
			ContactFieldContainer *getParent() const;

		protected:
			/**
			 * @return Field metadata.
			 */
			const ContactFieldMetadata &getMetadata() const;

			/**
			 * @brief Should be called to report fill state change.
			 * @param[in]   isFilled    Whether field is filled
			 */
			void onFilled(bool isFilled);

			/**
			 * @brief Called when field is being initialized.
			 * @param[in]   record      Record that initializes the field
			 */
			virtual void onInitialize(contacts_record_h record) { }

		private:
			contacts_record_h m_Record;
			const ContactFieldMetadata &m_Metadata;
			ContactFieldContainer *m_Parent;
			FillCallback m_OnFilled;
		};

		typedef std::unique_ptr<ContactField> ContactFieldPtr;
		typedef std::vector<ContactFieldPtr> ContactFields;
	}
}

#endif /* CONTACTS_MODEL_CONTACT_FIELD_H */
