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

#ifndef CONTACTS_LIST_PERSON_GROUP_ITEM_H
#define CONTACTS_LIST_PERSON_GROUP_ITEM_H

#include "Ui/GenlistGroupItem.h"
#include "Utils/UniString.h"
#include <string>

namespace Contacts
{
	namespace List
	{
		/**
		 * @brief Person alphabet group item
		 */
		class PersonGroupItem : public Ui::GenlistGroupItem
		{
		public:
			/**
			 * @brief Create person group item
			 * @param[in]   title       Group title
			 * @param[in]   indexItem   Index item
			 */
			explicit PersonGroupItem(Utils::UniString title);
			virtual ~PersonGroupItem() override;

			/**
			 * @return Group item title
			 */
			const Utils::UniString &getTitle() const;

			/**
			 * @return Linked index item
			 */
			Elm_Object_Item *getIndexItem() const;

			/**
			 * @brief Link index item
			 */
			void setIndexItem(Elm_Object_Item *item);

		private:
			virtual char *getText(Evas_Object *parent, const char *part) override;

			Utils::UniString m_Title;
			Elm_Object_Item *m_IndexItem;
		};
	}
}

#endif /* CONTACTS_LIST_PERSON_GROUP_ITEM_H */
