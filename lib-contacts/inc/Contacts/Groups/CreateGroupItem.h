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

#ifndef CONTACTS_GROUPS_CREATE_GROUP_ITEM_H
#define CONTACTS_GROUPS_CREATE_GROUP_ITEM_H

#include "Ui/GenlistItem.h"

namespace Contacts
{
	namespace Groups
	{
		/**
		 * @brief "Create Group" genlist item.
		 */
		class CreateGroupItem : public Ui::GenlistItem
		{
		public:
			/**
			 * @brief Called when new group is created.
			 * @param[in]   New group ID
			 */
			typedef std::function<void(int)> ResultCallback;

			/**
			 * @brief Set group creation result callback
			 * @param[in]   callback    Result callback
			 */
			void setResultCallback(ResultCallback callback);

		private:
			virtual Elm_Genlist_Item_Class *getItemClass() const override;
			virtual Evas_Object *getContent(Evas_Object *parent, const char *part) override;
			void onButtonPressed(Evas_Object *button, void *eventInfo);

			ResultCallback m_OnResult;
		};
	}
}

#endif /* CONTACTS_GROUPS_CREATE_GROUP_ITEM_H */
