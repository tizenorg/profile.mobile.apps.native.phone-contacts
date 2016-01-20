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

ContactTextFieldControl::ContactTextFieldControl(Model::ContactTextField &field)
	: m_Field(field), m_IsEmpty(true)
{
}

void ContactTextFieldControl::save()
{
	char *text = elm_entry_markup_to_utf8(elm_entry_entry_get(getEntry()));
	m_Field.setValue(text);
	free(text);
}

void ContactTextFieldControl::update()
{
	char *text = elm_entry_utf8_to_markup(m_Field.getValue());
	elm_entry_entry_set(getEntry(), text);
	free(text);
}

void ContactTextFieldControl::onCreated()
{
	setGuideText(Common::getContactChildFieldName(m_Field.getId()));

	Evas_Object *entry = getEntry();
	elm_entry_input_panel_layout_set(entry, Utils::at(inputLayout, m_Field.getSubType()));
	evas_object_smart_callback_add(entry, "changed",
			makeCallback(&ContactTextFieldControl::onChanged), this);
	evas_object_smart_callback_add(entry, "unfocused",
			makeCallback(&ContactTextFieldControl::onUnfocused), this);

	update();
}

void ContactTextFieldControl::onChanged(Evas_Object *entry, void *eventInfo)
{
	if (m_IsEmpty != elm_entry_is_empty(entry)) {
		m_IsEmpty = !m_IsEmpty;
		save();
	}
}

void ContactTextFieldControl::onUnfocused(Evas_Object *entry, void *eventInfo)
{
	save();
}
