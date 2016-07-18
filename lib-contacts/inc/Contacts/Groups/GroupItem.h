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

#ifndef CONTACTS_GROUPS_GROUP_ITEM_H
#define CONTACTS_GROUPS_GROUP_ITEM_H

#include "Contacts/Groups/Model/Group.h"
#include "Ux/SelectItem.h"

#define PART_GROUP_NAME      "elm.text"
#define PART_GROUP_CHECK     "elm.swallow.end"

namespace Contacts
{
	namespace List
	{
		class ListView;
	}

	namespace Groups
	{
		/**
		 * @brief Group genlist item
		 */
		class GroupItem : public Ux::SelectItem
		{
		public:
			/**
			 * @brief Create group item
			 * @param[in]   group   Group data
			 */
			explicit GroupItem(Model::Group &group);

			/**
			 * @return Group data.
			 */
			Model::Group &getGroup() const;

			/**
			 * @brief Update item.
			 * @param[in]   changes Change info
			 */
			void update(int changes);

		private:
			virtual char *getText(Evas_Object *parent, const char *part) override;
			virtual Evas_Object *getContent(Evas_Object *parent, const char *part) override;
			virtual Ux::SelectResult getDefaultResult() const override;
			virtual void onSelected() override;

			Model::Group &m_Group;
		};
	}
}



#endif /* CONTACTS_GROUPS_GROUP_ITEM_H */
