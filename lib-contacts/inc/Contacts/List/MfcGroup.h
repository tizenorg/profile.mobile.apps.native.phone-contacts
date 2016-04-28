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

#ifndef CONTACTS_LIST_MFC_GROUP_H
#define CONTACTS_LIST_MFC_GROUP_H

#include "Contacts/List/Model/MfcProvider.h"
#include "Ui/GenlistGroupItem.h"

namespace Contacts
{
	namespace List
	{
		class PersonItem;

		/**
		 * @brief Represents the group containing Mfc (most frequent contacts) genlist items.
		 */
		class MfcGroup : public Ui::GenlistGroupItem
		{
		public:
			/**
			 * @brief Update group item callback.
			 * @param[in]   isEmpty If it is true mfc group item should be hidden, otherwise - shown
			 */
			typedef std::function<void(bool isEmpty)> UpdateCallback;

			MfcGroup();

			/**
			 * @brief Set update callback
			 * @param[in]   callback    Update callback
			 */
			void setUpdateCallback(UpdateCallback callback);

		private:
			virtual char *getText(Evas_Object *parent, const char *part) override;

			void onInserted(Contacts::Model::ContactData &person);
			void onDeleted(PersonItem *item);

			PersonItem *createItem(Model::Person &person);

			Model::MfcProvider m_Provider;
			UpdateCallback m_OnUpdated;
		};
	}
}



#endif /* CONTACTS_LIST_MFC_GROUP_H */
