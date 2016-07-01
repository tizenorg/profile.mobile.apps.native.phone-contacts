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

#include "Contacts/Groups/AddMembersItem.h"

#include "GroupItemLayout.h"

#include <app_i18n.h>

#define BUFFER_SIZE 32

using namespace Contacts::Groups;

AddMembersItem::AddMembersItem()
	: m_Count(0)
{
}

Elm_Genlist_Item_Class *AddMembersItem::getItemClass() const
{
	static Elm_Genlist_Item_Class itc = createItemClass(GROUP_ADD_MEMBERS_ITEM_STYLE);
	return &itc;
}

char *AddMembersItem::getText(Evas_Object *parent, const char *part)
{
	if (strcmp(part, PART_GROUP_ADD_MEMBERS_LABEL) == 0) {
		return strdup(_("IDS_PB_MBODY_ADD_MEMBERS_ABB"));
	} else if (strcmp(part, PART_GROUP_ADD_MEMBERS_COUNTER) == 0) {
		char counter[BUFFER_SIZE] = { 0, };
		snprintf(counter, sizeof(counter), "(%d)", m_Count);
		return strdup(counter);
	}
	return nullptr;
}

Evas_Object *AddMembersItem::getContent(Evas_Object *parent, const char *part)
{
	if (strcmp(part, PART_GROUP_ADD_MEMBERS_BUTTON) == 0) {
		Evas_Object *button = elm_button_add(parent);
		elm_object_style_set(button, "icon_expand_add");
		elm_object_focus_allow_set(button, EINA_FALSE);
		return button;
	}
	return nullptr;
}

void AddMembersItem::onSelected()
{
	//TODO: Member selector.
}
