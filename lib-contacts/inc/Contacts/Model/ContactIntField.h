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

#ifndef CONTACTS_MODEL_CONTACT_INT_FIELD_H
#define CONTACTS_MODEL_CONTACT_INT_FIELD_H

#include "Contacts/Model/ContactField.h"

namespace Contacts
{
	namespace Model
	{
		/**
		 * @brief Adaptor for property containing integer value
		 *
		 * @see TypeInt
		 */
		class ContactIntField : public ContactField
		{
		public:
			using ContactField::ContactField;

			/**
			 * @see ContactField::isChanged()
			 */
			virtual bool isChanged() const override;

			/**
			 * @return Field value.
			 */
			int getValue() const;

			/**
			 * @brief Set field value.
			 */
			void setValue(int value);

		protected:
			/**
			 * @see ContactField::onInitialize()
			 */
			virtual void onInitialize(contacts_record_h record) override;

		private:
			int m_InitialValue;
		};
	}
}

#endif /* CONTACTS_MODEL_CONTACT_INT_FIELD_H */
