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

#ifndef CONTACTS_INPUT_CONTACT_TEXT_FIELD_CONTROL_H
#define CONTACTS_INPUT_CONTACT_TEXT_FIELD_CONTROL_H

#include "Ui/Editfield.h"

namespace Contacts
{
	namespace Model
	{
		class ContactTextField;
	}

	namespace Input
	{
		/**
		 * @brief UI Control for editing ContactTextField
		 */
		class ContactTextFieldControl : public Ui::Editfield
		{
		public:
			/**
			 * @brief Create control
			 * @param[in]   field   Contact text field to edit
			 */
			ContactTextFieldControl(Model::ContactTextField &field);

		private:
			virtual void onCreated() override;
			void onUnfocused(Evas_Object *entry, void *eventInfo);

			Model::ContactTextField &m_Field;
		};
	}
}

#endif /* CONTACTS_INPUT_CONTACT_TEXT_FIELD_CONTROL_H */
