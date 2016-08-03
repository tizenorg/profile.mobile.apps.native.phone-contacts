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

#include "Common/Database/RecordUtils.h"
#include "Ui/Editfield.h"
#include "Utils/Callback.h"

#include <app_i18n.h>

using namespace Common::Database;
using namespace Contacts::Groups;

NameItem::NameItem(contacts_record_h record)
	: m_Record(record)
{
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
		return strdup(_("IDS_PB_MBODY_GROUP_NAME_ABB"));
	}
	return nullptr;
}

Evas_Object *NameItem::getContent(Evas_Object *parent, const char *part)
{
	if (strcmp(part, PART_GROUP_NAME_VALUE) == 0) {
		Ui::Editfield *edit = Ui::Editfield::create(parent, "IDS_PB_MBODY_GROUP_NAME_ABB");
		char *name = elm_entry_utf8_to_markup(getRecordStr(m_Record, _contacts_group.name));
		elm_entry_entry_set(edit->getEntry(), name);
		free(name);
		evas_object_smart_callback_add(edit->getEntry(), "changed",
				makeCallback(&NameItem::onChanged), this);
		return edit->getEvasObject();
	}
	return nullptr;
}

void NameItem::onInserted()
{
	elm_genlist_item_select_mode_set(getObjectItem(), ELM_OBJECT_SELECT_MODE_NONE);
}

void NameItem::onChanged(Evas_Object *entry, void *eventInfo)
{
	char *text = elm_entry_markup_to_utf8(elm_entry_entry_get(entry));
	contacts_record_set_str(m_Record, _contacts_group.name, text ? text : "");
	free(text);

	m_OnFilled(!elm_entry_is_empty(entry));
}
