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

#ifndef CONTACTS_INPUT_CONTACT_DATE_FIELD_CONTROL_H
#define CONTACTS_INPUT_CONTACT_DATE_FIELD_CONTROL_H

#include "Ui/Control.h"

namespace Contacts
{
	namespace Model
	{
		class ContactDateField;
	}

	namespace Input
	{
		/**
		 * @brief UI Control for editing ContactDateField
		 */
		class ContactDateFieldControl : public Ui::Control
		{
		public:
			/**
			 * @brief Create control
			 * @param[in]   field   Contact date field to edit
			 */
			ContactDateFieldControl(Model::ContactDateField *field);

		private:
			virtual Evas_Object *onCreate(Evas_Object *parent) override;
			virtual void onCreated() override;

			void setButtonDate(tm date);
			void onButtonPressed(Evas_Object *button, void *eventInfo);

			Model::ContactDateField *m_Field;
			Evas_Object *m_Button;
		};
	}
}

#endif /* CONTACTS_INPUT_CONTACT_DATE_FIELD_CONTROL_H */
