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

#include "Contacts/Input/ContactTextFieldControl.h"
#include "Contacts/Model/ContactTextField.h"
#include "Contacts/Common/Strings.h"

#include "Ui/GenlistItem.h"
#include "Utils/Callback.h"
#include "Utils/Range.h"

using namespace Contacts::Input;

namespace
{
	Elm_Input_Panel_Layout inputLayout[] = {
		/* [TextTypeRegular] = */ ELM_INPUT_PANEL_LAYOUT_NORMAL,
		/* [TextTypeNumber]  = */ ELM_INPUT_PANEL_LAYOUT_PHONENUMBER,
		/* [TextTypeEmail]   = */ ELM_INPUT_PANEL_LAYOUT_EMAIL,
		/* [TextTypeUrl]     = */ ELM_INPUT_PANEL_LAYOUT_URL
	};
}

ContactTextFieldControl::ContactTextFieldControl(Ui::GenlistItem *parent,
		Model::ContactTextField &field)
	: m_ParentItem(parent), m_NextItem(nullptr), m_Field(field),
	  m_IsEmpty(true), m_IsChanged(false), m_IsUpdating(false)
{
}

void ContactTextFieldControl::save()
{
	if (!m_IsChanged) {
		return;
	}

	char *text = elm_entry_markup_to_utf8(elm_entry_entry_get(getEntry()));
	if (text) {
		setFieldValue(text);
		free(text);
	}

	m_IsChanged = false;
}

void ContactTextFieldControl::update()
{
	m_IsUpdating = true;

	char *text = elm_entry_utf8_to_markup(getFieldValue().c_str());
	elm_entry_entry_set(getEntry(), text);
	free(text);
}

void ContactTextFieldControl::updateReturnKey()
{
	m_NextItem = nullptr;
	for (auto item = m_ParentItem->getNextItem(); item; item = item->getNextItem()) {
		if (item->isFocusable()) {
			m_NextItem = item;
			break;
		}
	}

	elm_entry_input_panel_return_key_type_set(getEntry(), m_NextItem
			? ELM_INPUT_PANEL_RETURN_KEY_TYPE_NEXT
			: ELM_INPUT_PANEL_RETURN_KEY_TYPE_DONE);
}

void ContactTextFieldControl::updateEntryLayout()
{
	setGuideText(Common::getContactChildFieldName(m_Field.getId()));
	elm_entry_input_panel_layout_set(getEntry(), Utils::at(inputLayout, m_Field.getSubType()));
}

std::string ContactTextFieldControl::getFieldValue() const
{
	const char *value = m_Field.getValue();
	return value ? value : "";
}

void ContactTextFieldControl::setFieldValue(std::string value)
{
	m_Field.setValue(value.c_str());
}

void ContactTextFieldControl::onCreated()
{
	Evas_Object *entry = getEntry();
	evas_object_smart_callback_add(entry, "changed",
			makeCallback(&ContactTextFieldControl::onChanged), this);
	evas_object_smart_callback_add(entry, "focused",
			makeCallback(&ContactTextFieldControl::onFocused), this);
	evas_object_smart_callback_add(entry, "unfocused",
			makeCallback(&ContactTextFieldControl::onUnfocused), this);
	evas_object_smart_callback_add(entry, "activated",
			makeCallback(&ContactTextFieldControl::onActivated), this);

	updateEntryLayout();
	update();
}

void ContactTextFieldControl::onChanged(Evas_Object *entry, void *eventInfo)
{
	if (m_IsUpdating) {
		m_IsUpdating = false;
	} else {
		m_IsChanged = true;
	}

	if (m_IsEmpty != elm_entry_is_empty(entry)) {
		m_IsEmpty = !m_IsEmpty;
		save();
	}
}

void ContactTextFieldControl::onFocused(Evas_Object *entry, void *eventInfo)
{
	updateReturnKey();
}

void ContactTextFieldControl::onUnfocused(Evas_Object *entry, void *eventInfo)
{
	save();
}

void ContactTextFieldControl::onActivated(Evas_Object *entry, void *eventInfo)
{
	if (m_NextItem) {
		m_NextItem->focus(ELM_GENLIST_ITEM_SCROLLTO_IN, true);
	} else {
		elm_object_focus_set(entry, EINA_FALSE);
	}
}
