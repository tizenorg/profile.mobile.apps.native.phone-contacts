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

#ifndef CONTACTS_GROUPS_MODEL_MEMBERS_PROVIDER_H
#define CONTACTS_GROUPS_MODEL_MEMBERS_PROVIDER_H

#include "Contacts/List/Model/PersonProvider.h"

namespace Contacts
{
	namespace Groups
	{
		namespace Model
		{
			class MembersProvider : public List::Model::PersonProvider
			{
			public:
				/**
			     * @brief Determines which items are displayed.
				 */
				enum Mode
				{
					ModeDefault,        /**< Group's members are displayed */
					ModeExclude         /**< Displayed all contacts which are not group's members */
				};

				/**
				 * @brief Provides list of group's members
				 * @param[in]   groupId     Group id
				 * @param[in]   mode        Members mode
				 */
				explicit MembersProvider(int groupId, Mode mode = ModeDefault);
				virtual ~MembersProvider() override;

			private:
				virtual contacts_filter_h getFilter() const override;
				virtual contacts_list_h getPersonList() const override;
				virtual contacts_record_h getPersonRecord(int id, IdType idType) const override;

				contacts_list_h getMembersList() const;

				int m_GroupId;
				Mode m_Mode;
			};
		}
	}
}

#endif /* CONTACTS_GROUPS_MODEL_MEMBERS_PROVIDER_H */
