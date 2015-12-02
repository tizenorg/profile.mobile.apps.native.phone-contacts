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

#ifndef CONTACTS_MODEL_CONTACT_TYPED_FIELD_H
#define CONTACTS_MODEL_CONTACT_TYPED_FIELD_H

#include "Contacts/Model/ContactField.h"

namespace Contacts
{
	namespace Model
	{
		class ContactEnumMetadata;

		/**
		 * @brief Adaptor for property containing one of the limited possible
		 *        integer values or a custom string value.
		 *
		 * @see TypeEnum
		 */
		class ContactEnumField : public ContactField
		{
		public:
			/**
			 * @brief ContactField::ContactField()
			 */
			ContactEnumField(contacts_record_h record,
					const ContactFieldMetadata *metadata);

			/**
			 * @see ContactField::reset()
			 */
			virtual void reset() override;

			/**
			 * @return Field value.
			 */
			int getValue() const;

			/**
			 * @brief Set field value.
			 * @param[in]   value   Field value
			 */
			void setValue(int value);

			/**
			 * @return Field custom value.
			 */
			const char *getCustomValue() const;

			/**
			 * @brief Set field custom value.
			 * @param[in]   label   Field custom value
			 */
			void setCustomValue(const char *value);

			/**
			 * @return Whether field has a custom value.
			 */
			bool hasCustomValue() const;

		private:
			const ContactEnumMetadata *getEnumMetadata() const;

			unsigned m_CustomValuePropId;
		};
	}
}

#endif /* CONTACTS_MODEL_CONTACT_TYPED_FIELD_H */
