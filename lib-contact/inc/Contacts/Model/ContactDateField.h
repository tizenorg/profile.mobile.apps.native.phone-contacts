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

#ifndef CONTACTS_MODEL_CONTACT_DATE_FIELD_H
#define CONTACTS_MODEL_CONTACT_DATE_FIELD_H

#include <time.h>
#include "Contacts/Model/ContactField.h"

namespace Contacts
{
	namespace Model
	{
		/**
		 * @brief Adaptor for property containing date value
		 *
		 * @see TypeDate
		 */
		class ContactDateField : public ContactField
		{
		public:
			using ContactField::ContactField;

			/**
			 * @see ContactInputField::reset()
			 */
			virtual void reset() override;

			/**
			 * @see ContactField::isChanged()
			 */
			virtual bool isChanged() const override;

			/**
			 * @return Date field value.
			 */
			tm getValue() const;

			/**
			 * @brief Set date field value.
			 * @param[in]   date    Date value
			 */
			void setValue(tm date);

		protected:
			/**
			 * @brief Get field value from the given record.
			 */
			tm getValue(contacts_record_h record) const;

			/**
			 * @see ContactField::onInitialize()
			 */
			virtual void onInitialize(contacts_record_h record) override;

		private:
			static tm convertDate(int value);
			static int convertDate(const tm &date);

			time_t m_InitialValue;
		};
	}
}

#endif /* CONTACTS_MODEL_CONTACT_DATE_FIELD_H */
