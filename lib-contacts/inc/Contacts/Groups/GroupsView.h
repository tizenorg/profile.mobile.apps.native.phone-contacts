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

#ifndef CONTACTS_GROUPS_GROUPS_VIEW_H
#define CONTACTS_GROUPS_GROUPS_VIEW_H

#include "Ui/View.h"
#include "Contacts/Groups/Model/Group.h"

#include "vector"

namespace Ui
{
	class Genlist;
}

namespace Contacts
{
	namespace Groups
	{
		class GroupItem;

		namespace Model
		{
			class GroupsProvider;
		}

		class GroupsView : public Ui::View
		{
		public:
			GroupsView();
			~GroupsView();

		private:
			virtual Evas_Object *onCreate(Evas_Object *parent) override;
			virtual void onPageAttached(Ui::NavigatorPage *page) override;

			void onInserted(::Model::DataItem &data);
			GroupItem *createItem(Model::Group &group);
			void insertItem(GroupItem *item);
			void updateItem(GroupItem *item, int changes);
			void deleteItem(GroupItem *item);
			GroupItem *getNextItem(Model::Group &group);

			Ui::Genlist *m_Genlist;
			Model::GroupsProvider *m_Provider;
		};
	}
}



#endif /* CONTACTS_GROUPS_GROUPS_VIEW_H */
