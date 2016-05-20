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

#ifndef CONTACTS_LIST_REORDER_ITEM_H
#define CONTACTS_LIST_REORDER_ITEM_H

#include "Contacts/List/ContactItem.h"

#define PART_CONTACT_REORDER       "elm.swallow.end"

namespace Contacts
{
	namespace Model
	{
		class ContactData;
	}

	namespace List
	{
		/**
		 * @brief Reorder genlist item
		 */
		class ReorderItem : public ContactItem
		{
		public:
			/**
			 * @brief Create reorder item
			 * @param[in]   contactData     Contact data
			 */
			explicit ReorderItem(Contacts::Model::ContactData &contactData);

		private:
			virtual Evas_Object *getContent(Evas_Object *parent, const char *part) override;

			void onMouseDown(Evas *evas, Evas_Object *obj, void *event_info);
		};
	}
}

#endif /* CONTACTS_LIST_REORDER_ITEM_H */
