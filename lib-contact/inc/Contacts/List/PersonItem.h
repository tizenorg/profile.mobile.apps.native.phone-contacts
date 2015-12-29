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

#include "Contacts/List/Model/Person.h"
#include "Ui/GenlistItem.h"
#include <contacts.h>

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
		class PersonItem : public Ui::GenlistItem
		{
			/**
			 * @brief Represents item mode
			 */
			enum ItemMode {
				DefaultMode,    /*< Usual mode */
				PickMode        /*< Mode with ability to select item */
			};
		public:
			/**
			 * @brief Create person item
			 * @param[in]   person      Person object
			 */
			PersonItem(Model::PersonPtr person);

			/**
			 * @brief Enable item usual mode
			 */
			void setDefaultMode();

			/**
			 * @brief Enable item pick mode
			 * @remark @state is a pointer, that we have to bind to item state
			 * @param[in]   state       State
			 */
			void setPickMode(Eina_Bool *state);

			/**
			 * @return Person object
			 */
			const Model::Person &getPerson() const;

			/**
			 * @brief Set person object
			 * @param[in]   person      Person object
			 */
			void setPerson(Model::PersonPtr person);

		private:
			virtual char *getText(Evas_Object *parent, const char *part) override;
			virtual Evas_Object *getContent(Evas_Object *parent, const char *part) override;

			Model::PersonPtr m_Person;
			ItemMode m_ItemMode;
			Eina_Bool *m_StatePointer;
		};
	}
}

#endif /* CONTACTS_LIST_PERSON_ITEM_H */
