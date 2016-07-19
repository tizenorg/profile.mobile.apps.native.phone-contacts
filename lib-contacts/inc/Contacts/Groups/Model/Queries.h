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

#ifndef CONTACTS_GROUPS_MODEL_QUERIES_H
#define CONTACTS_GROUPS_MODEL_QUERIES_H

#include <contacts.h>

namespace Contacts
{
	namespace Groups
	{
		namespace Model
		{
			/**
			 * @brief Get query for group's members filtered by group name.
			 * @remark Return value MUST be destroyed with contacts_query_destroy by you
			 * @param[in]   groupId     _contacts_group ID.
			 * @return filtered query.
			 */
			EXPORT_API contacts_query_h getMembersQuery(int groupId);

			/**
			 * @brief Get members count of specified group.
			 * @param[in]   groupId     _contacts_group ID.
			 * @return members count.
			 */
			EXPORT_API int getMembersCount(int groupId);
		}
	}
}



#endif /* CONTACTS_GROUPS_MODEL_QUERIES_H */
