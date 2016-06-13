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

#include "Contacts/Groups/GroupItem.h"

using namespace Contacts::Groups;
using namespace Contacts::Groups::Model;

GroupItem::GroupItem(Group &data)
	: m_Data(data)
{
}

Group &GroupItem::getGroupData() const
{
	return m_Data;
}

void GroupItem::update(int changes)
{
	if (changes & Group::ChangedName) {
		elm_genlist_item_fields_update(getObjectItem(),
				PART_GROUP_NAME, ELM_GENLIST_ITEM_FIELD_TEXT);
	}
}

char *GroupItem::getText(Evas_Object *parent, const char *part)
{
	if (strcmp(part, PART_GROUP_NAME) == 0) {
		const char *name = m_Data.getName();
		return strdup(name);
	}

	return nullptr;
}

Ux::SelectResult GroupItem::getDefaultResult() const
{
	return { 0, &m_Data };
}
