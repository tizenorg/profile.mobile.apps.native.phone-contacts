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

#include "Contacts/Model/ContactTextField.h"
#include "Ui/Editfield.h"
#include <string>

namespace Ui
{
	class GenlistItem;
}

namespace Contacts
{
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
			 * @param[in]   parent  Parent genlist item
			 * @param[in]   field   Contact text field to edit
			 */
			ContactTextFieldControl(Ui::GenlistItem *parent,
					Model::ContactTextField &field);
			virtual ~ContactTextFieldControl() override;

			/**
			 * @brief Save the control value into the field.
			 */
			void save();

			/**
			 * @brief Update the control using field's value.
			 */
			void update();

			/**
			 * @brief Update the state of the return key.
			 */
			void updateReturnKey();

		protected:
			/**
			 * @brief Update entry input layout and guide text.
			 */
			virtual void updateEntryLayout();

			/**
			 * @return Field value to display in entry.
			 */
			virtual std::string getFieldValue() const;

			/**
			 * @brief Set text field value.
			 * @param[in]   value   Value to store in the field
			 */
			virtual void setFieldValue(std::string value);

		private:
			virtual void onCreated() override;
			void setNextItem(Ui::GenlistItem *item);
			void unsetNextItem();

			void onChanged(Evas_Object *entry, void *eventInfo);
			void onFocused(Evas_Object *entry, void *eventInfo);
			void onUnfocused(Evas_Object *entry, void *eventInfo);
			void onActivated(Evas_Object *entry, void *eventInfo);

			Ui::GenlistItem *m_ParentItem;
			Ui::GenlistItem *m_NextItem;
			Model::ContactTextField &m_Field;

			bool m_IsEmpty;
			bool m_IsChanged;
			bool m_IsUpdating;
		};
	}
}

#endif /* CONTACTS_INPUT_CONTACT_TEXT_FIELD_CONTROL_H */
