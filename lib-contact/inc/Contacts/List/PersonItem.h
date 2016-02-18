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

#include "Contacts/Common/SelectMode.h"
#include "Contacts/List/Model/Person.h"
#include "Ui/GenlistCheckItem.h"
#include <functional>

#define PART_PERSON_NAME        "elm.text"
#define PART_PERSON_THUMBNAIL   "elm.swallow.icon"
#define PART_CHECK              "elm.swallow.end"

namespace Contacts
{
	namespace List
	{
		/**
		 * @brief Person list item
		 */
		class PersonItem : public Ui::GenlistCheckItem
		{
		public:
			/**
			 * @brief Create person item
			 * @param[in]   person      Person object
			 */
			explicit PersonItem(Model::PersonPtr person);

			/**
			 * @brief Set item selection mode
			 * @param[in]   selectMode  Item selection mode
			 */
			void setSelectMode(SelectMode selectMode);

			/**
			 * @return Person object
			 */
			Model::Person &getPerson();

			/**
			 * @brief Set person object
			 * @param[in]   person      Person object
			 */
			void setPerson(Model::PersonPtr person);

		private:
			virtual char *getText(Evas_Object *parent, const char *part) override;
			virtual Evas_Object *getContent(Evas_Object *parent, const char *part) override;

			Model::PersonPtr m_Person;
			SelectMode m_SelectMode;
		};
	}
}

#endif /* CONTACTS_LIST_PERSON_ITEM_H */
