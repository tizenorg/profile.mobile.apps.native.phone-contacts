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

#include "Contacts/List/ContactSearchItem.h"
#include "Utils/Callback.h"
#include "Utils/Logger.h"

using namespace Contacts::List;


ContactSearchItem::ContactSearchItem()
	: m_SearchField(nullptr)
{
}

Evas_Object *ContactSearchItem::getContent(Evas_Object *parent, const char *part)
{
	if (strcmp(part, "elm.swallow.content") == 0) {
		m_SearchField = Ui::Editfield::create(parent, "IDS_PB_NPBODY_SEARCH");

		Evas_Object *entry = m_SearchField->getEntry();
		evas_object_propagate_events_set(m_SearchField->getEvasObject(), EINA_TRUE);
		elm_entry_input_panel_return_key_autoenabled_set(entry, EINA_TRUE);
		elm_entry_input_panel_return_key_type_set(entry, ELM_INPUT_PANEL_RETURN_KEY_TYPE_DONE);
		evas_object_smart_callback_add(entry, "changed",
			makeCallback(&ContactSearchItem::onChangedPressed), this);
		evas_object_smart_callback_add(entry, "activated",
			makeCallback(&ContactSearchItem::onDonePressed), this);
		return m_SearchField->getEvasObject();
	}
	return nullptr;
}

Elm_Genlist_Item_Class *ContactSearchItem::getItemClass() const
{
	static Elm_Genlist_Item_Class itc = createItemClass("full");
	return &itc;
}

void ContactSearchItem::setEntryChangedCb(ChangedCallback callback)
{
	m_onChanged = std::move(callback);
}

void ContactSearchItem::setDonePressedCb(ChangedCallback callback)
{
	m_onDone = std::move(callback);
}

void ContactSearchItem::onDonePressed(Evas_Object *entry, void *eventInfo)
{
	if (m_onDone) {
		m_onDone();
	}
	elm_object_focus_set(entry, EINA_FALSE);
}

void ContactSearchItem::onChangedPressed(Evas_Object *entry, void *eventInfo)
{
	if (m_onChanged) {
		m_onChanged();
	}
}
