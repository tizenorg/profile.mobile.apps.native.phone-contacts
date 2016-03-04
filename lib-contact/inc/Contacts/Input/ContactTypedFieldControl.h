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

#ifndef CONTACTS_INPUT_CONTACT_TYPED_FIELD_CONTROL_H
#define CONTACTS_INPUT_CONTACT_TYPED_FIELD_CONTROL_H

#include "Ui/Hoversel.h"

namespace Contacts
{
	namespace Model
	{
		class ContactEnumField;
		class ContactTextField;
	}

	namespace Input
	{
		/**
		 * @brief UI control for displaying and editing type of typed field.
		 *
		 * @see ContactTypedObject
		 */
		class ContactTypedFieldControl : public Ui::Hoversel
		{
		public:
			/**
			 * @brief Create type control
			 * @param[in]   typeField   Object's "type" field
			 * @param[in]   labelField  Object's custom type label field
			 */
			ContactTypedFieldControl(Model::ContactEnumField &typeField,
					Model::ContactTextField &labelField);

		private:
			virtual void onCreated() override;
			bool onSelected(int value);

			Model::ContactEnumField &m_TypeField;
			Model::ContactTextField &m_LabelField;
		};
	}
}

#endif /* CONTACTS_INPUT_CONTACT_TYPED_FIELD_CONTROL_H */
