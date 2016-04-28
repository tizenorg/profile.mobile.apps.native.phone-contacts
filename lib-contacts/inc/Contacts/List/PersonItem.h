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

#ifndef CONTACTS_LIST_PERSON_ITEM_H
#define CONTACTS_LIST_PERSON_ITEM_H

#include "Contacts/List/ContactItem.h"

namespace Contacts
{
	namespace List
	{
		namespace Model
		{
			class Person;
		}

		/**
		 * @brief Person genlist item
		 */
		class PersonItem : public ContactItem
		{
		public:
			/**
			 * @brief Create person item
			 * @param[in]   person      Person object
			 */
			explicit PersonItem(Model::Person &person);

			/**
			 * @return Person object.
			 */
			Model::Person &getPerson() const;

		private:
			virtual Ux::SelectResult getDefaultResult() const override;
			virtual void onSelected() override;
		};
	}
}

#endif /* CONTACTS_LIST_PERSON_ITEM_H */
