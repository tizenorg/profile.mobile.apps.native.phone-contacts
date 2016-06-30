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

#include "Contacts/Groups/NameItem.h"

#include "GroupItemLayout.h"
#include "Ui/Editfield.h"

using namespace Contacts::Groups;

NameItem::NameItem(std::string name)
	: m_Edit(nullptr), m_Name(name)
{
}

std::string NameItem::getName()
{
	return m_Name;
}

void NameItem::setFilledCallback(FilledCallback callback)
{
	m_OnFilled = std::move(callback);
}

Elm_Genlist_Item_Class *NameItem::getItemClass() const
{
	static Elm_Genlist_Item_Class itc = createItemClass(GROUP_NAME_ITEM_STYLE);
	return &itc;
}

char *NameItem::getText(Evas_Object *parent, const char *part)
{
	if (strcmp(part, PART_GROUP_NAME_LABEL) == 0) {
		return strdup("Group Name");
	}
	return nullptr;
}

Evas_Object *NameItem::getContent(Evas_Object *parent, const char *part)
{
	if (strcmp(part, PART_GROUP_NAME_VALUE) == 0) {
		m_Edit = Ui::Editfield::create(parent, "Group Name");
		elm_entry_entry_set(m_Edit->getEntry(), m_Name.c_str());
		evas_object_smart_callback_add(m_Edit->getEntry(), "changed", (Evas_Smart_Cb)onChanged, this);
		return m_Edit->getEvasObject();
	}
	return nullptr;
}

void NameItem::onChanged(NameItem *item, Evas_Object *entry, void *eventInfo)
{
	char *text = elm_entry_markup_to_utf8(elm_entry_entry_get(entry));
	item->m_Name = text ? text : "";
	free(text);

	item->m_OnFilled(!elm_entry_is_empty(entry));
	Ui::Editfield::onChanged(item->m_Edit->getEvasObject(), entry, eventInfo);
}
