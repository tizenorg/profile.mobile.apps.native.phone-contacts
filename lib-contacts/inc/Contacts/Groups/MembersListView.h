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

#ifndef CONTACTS_GROUPS_MEMBERS_LIST_VIEW_H
#define CONTACTS_GROUPS_MEMBERS_LIST_VIEW_H

#include "Contacts/List/ListView.h"

namespace Contacts
{
	namespace Groups
	{
		/**
		 * @brief Group members list view.
		 */
		class MembersListView : public List::ListView
		{
		public:
			/**
			 * @brief Create members list view.
			 * @param[in]   groupId     Group id
			 */
			MembersListView(int groupId);

		private:
			virtual void onPageAttached(Ui::NavigatorPage *page) override;
			virtual void onMenuPressed() override;
			virtual void onUpdateFinished() override;

			std::string getTitle() const;

			int m_GroupId;
		};
	}
}

#endif /* CONTACTS_GROUPS_MEMBERS_LIST_VIEW_H */
