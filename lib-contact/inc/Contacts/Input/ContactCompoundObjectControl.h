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

#ifndef CONTACTS_INPUT_CONTACT_COMPOUND_OBJECT_CONTROL_H
#define CONTACTS_INPUT_CONTACT_COMPOUND_OBJECT_CONTROL_H

#include "Ui/Editfield.h"

namespace Contacts
{
	namespace Model
	{
		class ContactCompoundObject;
	}

	namespace Input
	{
		/**
		 * @brief UI Control for editing ContactCompoundObject
		 */
		class ContactCompoundObjectControl : public Ui::Editfield
		{
		public:
			/**
			 * @brief Create control
			 * @param[in]   object  Contact compound object
			 */
			ContactCompoundObjectControl(Model::ContactCompoundObject &object);

		private:
			virtual void onCreated() override;
			void onUnfocused(Evas_Object *entry, void *eventInfo);

			Model::ContactCompoundObject &m_Object;
		};
	}
}

#endif /* CONTACTS_INPUT_CONTACT_COMPOUND_OBJECT_CONTROL_H */
