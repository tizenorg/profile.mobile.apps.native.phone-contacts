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

#ifndef CONTACTS_GROUPS_ADD_MEMBERS_ITEM_H
#define CONTACTS_GROUPS_ADD_MEMBERS_ITEM_H

#include "Ui/GenItem.h"
#include "Ux/SelectTypes.h"

#include <vector>

namespace Ui
{
	class ProcessPopup;
}

namespace Contacts
{
	namespace List
	{
		class ListView;
	}

	namespace Groups
	{
		/**
		 * @brief Genlist item representing group's add members item
		 */
		class AddMembersItem : public Ui::GenItem
		{
		public:
			/**
			 * @brief Constructor.
			 * param[in]   groupId  Group id
			 */
			AddMembersItem(int groupId);

			/**
			 * @return the list of contact id added by user
			 */
			const std::vector<int> &getMemberIdList() const;

		private:
			virtual Elm_Genlist_Item_Class *getItemClass() const override;
			virtual char *getText(Evas_Object *parent, const char *part) override;
			virtual Evas_Object *getContent(Evas_Object *parent, const char *part) override;
			virtual void onSelected() override;
			virtual void onInserted() override;

			bool onMembersSelected(List::ListView *view, Ux::SelectResults results);
			void addMembers(Ux::SelectResults results);
			void onAddMembersFinished(List::ListView *view, Ui::ProcessPopup *popup);

			int m_GroupId;
			int m_Count;
			std::vector<int> m_ContactIdList;
		};
	}
}

#endif /* CONTACTS_GROUPS_ADD_MEMBERS_ITEM_H */
