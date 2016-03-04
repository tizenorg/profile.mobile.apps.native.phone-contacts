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

#include "Contacts/Input/ContactTypedFieldLabelPopup.h"
#include "Ui/Editfield.h"
#include "Utils/Callback.h"

using namespace Contacts::Input;

ContactTypedFieldLabelPopup::ContactTypedFieldLabelPopup()
	: m_Editfield(nullptr)
{
}

void ContactTypedFieldLabelPopup::setResultCallback(ResultCallback callback)
{
	m_OnResult = std::move(callback);
}

void ContactTypedFieldLabelPopup::onCreated()
{
	setTitle("IDS_PB_HEADER_ENTER_CUSTOM_LABEL_ABB");

	m_Editfield = Ui::Editfield::create(getEvasObject(), "IDS_MF_POP_INPUT_TEXT");
	setContent(m_Editfield->getEvasObject());
	addButton("IDS_PB_BUTTON_CANCEL");
	Evas_Object *button = addButton("IDS_PB_SK_CREATE_ABB", [this] {
		onCreatePressed();
		return true;
	});
	elm_object_disabled_set(button, EINA_TRUE);

	Evas_Object *entry = m_Editfield->getEntry();
	elm_entry_input_panel_return_key_autoenabled_set(entry, EINA_TRUE);
	elm_entry_input_panel_return_key_type_set(entry, ELM_INPUT_PANEL_RETURN_KEY_TYPE_DONE);
	evas_object_smart_callback_add(entry, "changed",
			(Evas_Smart_Cb) &ContactTypedFieldLabelPopup::onEntryChanged, button);
	evas_object_smart_callback_add(entry, "activated",
			makeCallback(&ContactTypedFieldLabelPopup::onDonePressed), this);
	elm_object_focus_set(entry, EINA_TRUE);
}

void ContactTypedFieldLabelPopup::onCreatePressed()
{
	if (m_OnResult) {
		char *text = elm_entry_markup_to_utf8(elm_entry_entry_get(m_Editfield->getEntry()));
		m_OnResult(text);
		free(text);
	}
}

void ContactTypedFieldLabelPopup::onDonePressed(Evas_Object *entry, void *eventInfo)
{
	onCreatePressed();
	delete this;
}

void ContactTypedFieldLabelPopup::onEntryChanged(Evas_Object *button, Evas_Object *entry, void *eventInfo)
{
	elm_object_disabled_set(button, elm_entry_is_empty(entry));
}
