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
#ifndef CONTACTS_LIST_CONTACT_GROUP_ITEM_H
#define CONTACTS_LIST_CONTACT_GROUP_ITEM_H

#include "Ui/GenlistGroupItem.h"
#include <string>

namespace Contacts
{
	namespace List
	{
		/**
		 * @brief Contact alphabet group item
		 */
		class ContactGroupItem : public Ui::GenlistGroupItem
		{
		public:
			/**
			 * @brief Create contact group item
			 * @param[in]   title       Group title (UTF-8 letter)
			 * @param[in]   indexItem   Index item
			 */
			ContactGroupItem(const char *title, Elm_Object_Item *indexItem);

			/**
			 * @return Group item title
			 */
			const char *getTitle() const;

			/**
			 * @return Linked index item
			 */
			Elm_Object_Item *getIndexItem() const;

		private:
			virtual char *getText(Evas_Object *parent, const char *part);

			std::string m_Title;
			Elm_Object_Item *m_IndexItem;
		};
	}
}

#endif /* CONTACTS_LIST_CONTACT_GROUP_ITEM_H */
