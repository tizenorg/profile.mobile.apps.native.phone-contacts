/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "Contacts/List/SearchItem.h"
#include "Ui/Editfield.h"
#include "Utils/Callback.h"
#include "Utils/Logger.h"

using namespace Contacts::List;

Evas_Object *SearchItem::getContent(Evas_Object *parent, const char *part)
{
	Ui::Editfield *searchField = Ui::Editfield::create(parent, "IDS_PB_NPBODY_SEARCH");

	Evas_Object *entry = searchField->getEntry();
	elm_entry_input_panel_return_key_autoenabled_set(entry, EINA_TRUE);
	elm_entry_input_panel_return_key_type_set(entry, ELM_INPUT_PANEL_RETURN_KEY_TYPE_DONE);
	evas_object_smart_callback_add(entry, "changed",
			makeCallback(&SearchItem::onChanged), this);
	evas_object_smart_callback_add(entry, "activated",
			makeCallback(&SearchItem::onDonePressed), this);
	return searchField->getEvasObject();
}

Elm_Genlist_Item_Class *SearchItem::getItemClass() const
{
	static Elm_Genlist_Item_Class itc = createItemClass("full");
	return &itc;
}

void SearchItem::setChangeCallback(ChangeCallback callback)
{
	m_OnChanged = std::move(callback);
}

void SearchItem::onDonePressed(Evas_Object *entry, void *eventInfo)
{
	elm_object_focus_set(entry, EINA_FALSE);
}

void SearchItem::onChanged(Evas_Object *entry, void *eventInfo)
{
	if (m_OnChanged) {
		m_OnChanged();
	}
}
