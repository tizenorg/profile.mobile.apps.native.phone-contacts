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

#include "Ux/SelectView.h"
#include "Contacts/Groups/Model/GroupsProvider.h"

namespace Ui
{
	class Genlist;
	class GenlistItem;
}

namespace Contacts
{
	namespace Groups
	{
		class GroupItem;
		class GroupsItem;
		class CreateGroupItem;

		namespace Model
		{
			class Group;
		}

		/**
		 * @brief Groups main view.
		 */
		class GroupsView : public Ux::SelectView
		{
		public:
			GroupsView();

			/**
			 * @brief Set whether the view is used for assigning contacts to groups.
			 * @param[in]   isEnabled   Whether "Assign Mode" is enabled
			 */
			void setAssignMode(bool isEnabled);

		private:
			virtual Evas_Object *onCreate(Evas_Object *parent) override;
			virtual void onMenuPressed() override;
			virtual void onSelectAllInsert(Ui::GenlistItem *item) override;
			void updateCreateItem();

			void onInserted(::Model::DataItem &data);
			GroupItem *createItem(Model::Group &group);
			void insertItem(GroupItem *item);
			void updateItem(GroupItem *item, int changes);
			void deleteItem(GroupItem *item);
			Ui::GenlistItem *getNextItem(Model::Group &group);

			Ui::Genlist *m_Genlist;
			CreateGroupItem *m_CreateItem;
			Model::GroupsProvider m_Provider;
			GroupsItem *m_GroupsItem;

			bool m_IsAssignMode;
			int m_NewGroupId;
		};
	}
}

#endif /* CONTACTS_GROUPS_GROUPS_VIEW_H */
