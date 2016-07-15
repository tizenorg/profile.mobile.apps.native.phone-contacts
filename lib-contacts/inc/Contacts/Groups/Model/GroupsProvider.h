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

#ifndef CONTACTS_GROUPS_MODEL_GROUPS_PROVIDER_H
#define CONTACTS_GROUPS_MODEL_GROUPS_PROVIDER_H

#include "Model/DataProvider.h"

#include <contacts.h>

namespace Contacts
{
	namespace Groups
	{
		namespace Model
		{
			class Group;

			class GroupsProvider : public ::Model::DataProvider
			{
			public:
				GroupsProvider();
				virtual ~GroupsProvider() override;

				/**
				 * @brief Get group list
				 * @return List of group objects
				 */
				virtual const DataList &getDataList() override final;
				virtual void clearDataList() override final;

			private:
				void onChanged(const char *uri);
				void onMembersChanged(const char *uri);

				void insertGroup(int id);
				void updateGroup(DataList::const_iterator groupIt);
				void deleteGroup(DataList::const_iterator groupIt);

				Group *createGroup(contacts_record_h record);
				DataList::const_iterator findGroup(int id);

				int m_DbVersion;
				int m_MembersDbVersion;
				DataList m_List;
				bool m_IsFilled;
			};
		}
	}
}



#endif /* CONTACTS_GROUPS_MODEL_GROUPS_PROVIDER_H */
