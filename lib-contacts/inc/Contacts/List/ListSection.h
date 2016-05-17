/*
 * Copyright (c) 2015-2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef CONTACTS_LIST_LIST_SECTION_H
#define CONTACTS_LIST_LIST_SECTION_H

#include "Ui/GenlistGroupItem.h"

namespace Contacts
{
	namespace Model
	{
		class ContactData;
	}

	namespace List
	{
		namespace Model
		{
			class Person;
			class PersonProvider;
		}
		class PersonItem;

		/**
		 * @brief Represents list section containing genlist items.
		 */
		class ListSection : public Ui::GenlistGroupItem
		{
		public:
			/**
			 * @brief Update group item callback.
			 * @param[in]   isEmpty If it is true group item should be hidden, otherwise - shown
			 */
			typedef std::function<void(bool isEmpty)> UpdateCallback;

			/**
			 * @brief Create list section
			 * @param[in]   title       Section title
			 * @param[in]   provider    Section provider
			 *
			 */
			ListSection(std::string title, Model::PersonProvider *provider);
			virtual ~ListSection() override;

			/**
			 * @brief Set update callback
			 * @param[in]   callback    Update callback
			 */
			void setUpdateCallback(UpdateCallback callback);

		protected:
			void onInserted(Contacts::Model::ContactData &person);
			PersonItem *createItem(Model::Person &person);

		private:
			virtual char *getText(Evas_Object *parent, const char *part) override;

			void onDeleted(PersonItem *item);

			std::string m_Title;
			UpdateCallback m_OnUpdated;
			Model::PersonProvider *m_Provider;
		};
	}
}

#endif /* CONTACTS_LIST_LIST_SECTION_H */
