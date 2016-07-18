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

#ifndef CONTACTS_INPUT_CONTACT_COMPOUND_FIELD_CONTROL_H
#define CONTACTS_INPUT_CONTACT_COMPOUND_FIELD_CONTROL_H

#include "Contacts/Input/ContactTextFieldControl.h"
#include "Contacts/Model/ContactCompoundObject.h"

namespace Contacts
{
	namespace Input
	{
		/**
		 * @brief UI Control for editing ContactCompoundObject
		 */
		class ContactCompoundFieldControl : public ContactTextFieldControl
		{
		public:
			/**
			 * @brief Create control
			 * @param[in]   parent  Parent genlist item
			 * @param[in]   object  Contact compound object
			 */
			ContactCompoundFieldControl(Ui::GenItem *parent,
					Model::ContactCompoundObject &object);

			/**
			 * @brief Set whether to display compound value or first field value.
			 * @param[in]   isEnabled   Whether compound mode is enabled
			 */
			void setCompoundMode(bool isEnabled);

		protected:
			virtual void updateEntryLayout() override;
			virtual std::string getFieldValue() const override;
			virtual void setFieldValue(std::string value) override;

		private:
			Model::ContactCompoundObject &m_Object;

			bool m_IsCompoundMode;
		};
	}
}

#endif /* CONTACTS_INPUT_CONTACT_COMPOUND_FIELD_CONTROL_H */
