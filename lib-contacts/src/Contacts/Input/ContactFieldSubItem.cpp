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
	: m_Field(field)
{
}

ContactField &ContactFieldSubItem::getField() const
{
	return m_Field;
}

Ui::Control *ContactFieldSubItem::getFieldControl() const
{
	Evas_Object *content = elm_object_item_part_content_get(getObjectItem(), PART_MIDDLE);
	return Ui::Control::getControl(content);
}

bool ContactFieldSubItem::isFocusable() const
{
	return m_Field.getType() == TypeText;
}

void ContactFieldSubItem::update()
{
	Ui::Control *control = getFieldControl();
	if (control) {
		if (m_Field.getType() == TypeText) {
			auto textControl = static_cast<ContactTextFieldControl *>(control);
			textControl->update();
		} else if (m_Field.getType() == TypeDate) {
			auto dateControl = static_cast<ContactDateFieldControl *>(control);
			dateControl->update();
		}
	}
}

Elm_Genlist_Item_Class *ContactFieldSubItem::getItemClass() const
{
	static Elm_Genlist_Item_Class itc = createItemClass(INPUT_ITEM_STYLE);
	return &itc;
}

Evas_Object *ContactFieldSubItem::getContent(Evas_Object *parent, const char *part)
{
	Ui::Control *control = nullptr;
	if (strcmp(part, PART_MIDDLE) == 0) {
		switch (m_Field.getType()) {
			case TypeText:
				control = new ContactTextFieldControl(this, m_Field.cast<ContactTextField>());
				break;
			case TypeDate:
				control = new ContactDateFieldControl(m_Field.cast<ContactDateField>());
				break;
			default:
				break;
		}
	}

	return control ? control->create(parent) : nullptr;
}

Eina_Bool ContactFieldSubItem::getState(Evas_Object *parent, const char *part)
{
	ContactFieldItem *parentItem = static_cast<ContactFieldItem *>(getParentItem());
	return parentItem ? parentItem->getState(parent, part) : EINA_FALSE;
}

void ContactFieldSubItem::onInserted()
{
	elm_genlist_item_select_mode_set(getObjectItem(), ELM_OBJECT_SELECT_MODE_NONE);
}

void ContactFieldSubItem::onFocused()
{
	Ui::Control *control = getFieldControl();
	if (control) {
		if (m_Field.getType() == TypeText) {
			auto textControl = static_cast<ContactTextFieldControl *>(control);
			elm_object_focus_set(textControl->getEntry(), EINA_TRUE);
		} else if (m_Field.getType() == TypeDate) {
			auto dateControl = static_cast<ContactDateFieldControl *>(control);
			dateControl->showPicker();
		}
	}
}
