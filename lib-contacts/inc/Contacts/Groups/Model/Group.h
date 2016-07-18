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

#ifndef CONTACTS_GROUPS_MODEL_GROUP_H
#define CONTACTS_GROUPS_MODEL_GROUP_H

#include "Model/DataItem.h"
#include "Utils/UniString.h"

#include <contacts.h>

namespace Contacts
{
	namespace Groups
	{
		namespace Model
		{
			class Group : public ::Model::DataItem
			{
			public:
				/**
				 * @brief Represents Group changes on update event
				 */
				enum ChangedInfo
				{
					ChangedNone         = 0,    /**< Nothing was changed */
					ChangedName         = 1,    /**< Name has changed */
					ChangedMembersCount = 2,    /**< Members count has changed */
				};

				/**
				 * @brief Create group object
				 * @param[in]   record  _contacts_group record
				 */
				Group(contacts_record_h record);
				virtual ~Group() override;

				/**
				 * @return Person ID
				 */
				int getId() const;

				/**
				 * @return Name
				 */
				const char *getName() const;

				/**
				 * @return Ringtone path
				 */
				const char *getRingtone() const;

				/**
				 * @return Members count
				 */
				const int getMembersCount() const;

				/**
				 * @return Group record
				 */
				contacts_record_h getRecord() const;

				/**
				 * @brief Compares group's values
				 * @return True if sort value less than in @a that, otherwise false
				 */
				bool operator<(const Group &that) const;

			private:
				friend class GroupsProvider;

				const Utils::UniString &getSortValue() const;
				void update(contacts_record_h record);
				void updateMembersCount();

				contacts_record_h m_Record;
				mutable Utils::UniString m_SortValue;
				int m_MembersCount;
			};
		}
	}
}

#endif /* CONTACTS_GROUPS_MODEL_GROUP_H */
