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

#include "Contacts/Input/ContactFieldSubItem.h"
#include "Contacts/Input/ContactFieldItem.h"
#include "Contacts/Input/ContactDateFieldControl.h"
#include "Contacts/Input/ContactTextFieldControl.h"
#include "Contacts/Model/ContactField.h"
#include "Utils/Callback.h"

#include "InputItemLayout.h"

using namespace Contacts::Input;
using namespace Contacts::Model;

ContactFieldSubItem::ContactFieldSubItem(ContactField &field)
	: m_Field(field), m_NextFocusItem(nullptr)
{
}

ContactField &ContactFieldSubItem::getField() const
{
	return m_Field;
}

bool ContactFieldSubItem::isFocusable() const
{
	return m_Field.getType() == TypeText;
}

Elm_Genlist_Item_Class *ContactFieldSubItem::getItemClass() const
{
	static Elm_Genlist_Item_Class itc = createItemClass(INPUT_ITEM_STYLE);
	return &itc;
}

Evas_Object *ContactFieldSubItem::getContent(Evas_Object *parent, const char *part)
{
	if (strcmp(part, PART_MIDDLE) == 0) {
		switch (m_Field.getType()) {
			case TypeText:
			{
				auto control = new ContactTextFieldControl(m_Field.cast<ContactTextField>());
				control->create(parent);

				enableEntryReturnButton(control->getEntry());
				return control->getEvasObject();
			}
			case TypeDate:
			{
				auto control = new ContactDateFieldControl(m_Field.cast<ContactDateField>());
				return control->create(parent);
			}
			default:
				break;
		}
	}

	return nullptr;
}

Eina_Bool ContactFieldSubItem::getState(Evas_Object *parent, const char *part)
{
	ContactFieldItem *parentItem = static_cast<ContactFieldItem *>(getParentItem());
	return parentItem ? parentItem->getState(parent, part) : EINA_FALSE;
}

void ContactFieldSubItem::onFocused()
{
	Evas_Object *content = elm_object_item_part_content_get(getObjectItem(), PART_MIDDLE);
	Ui::Control *control = Ui::Control::getControl(content);

	if (control) {
		if (m_Field.getType() == TypeText) {
			Evas_Object *entry = static_cast<Ui::Editfield *>(control)->getEntry();
			elm_object_focus_set(entry, EINA_TRUE);
		}
	}
}

void ContactFieldSubItem::enableEntryReturnButton(Evas_Object *entry)
{
	evas_object_smart_callback_add(entry, "focused",
			makeCallback(&ContactFieldSubItem::onEntryFocused), this);
	evas_object_smart_callback_add(entry, "activated",
			makeCallback(&ContactFieldSubItem::onEntryActivated), this);
}

void ContactFieldSubItem::onEntryFocused(Evas_Object *entry, void *eventInfo)
{
	m_NextFocusItem = nullptr;
	for (auto item = getNextItem(); item; item = item->getNextItem()) {
		if (item->isFocusable()) {
			m_NextFocusItem = item;
			break;
		}
	}

	elm_entry_input_panel_return_key_type_set(entry, m_NextFocusItem
			? ELM_INPUT_PANEL_RETURN_KEY_TYPE_NEXT
			: ELM_INPUT_PANEL_RETURN_KEY_TYPE_DONE);
}

void ContactFieldSubItem::onEntryActivated(Evas_Object *entry, void *eventInfo)
{
	if (m_NextFocusItem) {
		m_NextFocusItem->focus(ELM_GENLIST_ITEM_SCROLLTO_IN, true);
	} else {
		elm_object_focus_set(entry, EINA_FALSE);
	}
}
