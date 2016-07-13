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

#include "Ux/EditfieldPopup.h"
#include "Ui/Editfield.h"
#include "Utils/Callback.h"

using namespace Ui;
using namespace Ux;

EditfieldPopup::EditfieldPopup()
	: m_Editfield(nullptr), m_Strings{ nullptr }
{
}

void EditfieldPopup::setStrings(Strings strings)
{
	m_Strings = strings;
}

void EditfieldPopup::setResultCallback(ResultCallback callback)
{
	m_OnResult = std::move(callback);
}

Editfield *EditfieldPopup::getEditfield() const
{
	return m_Editfield;
}

void EditfieldPopup::onCreated()
{
	m_Editfield = Editfield::create(getEvasObject(), m_Strings.guideText);

	setTitle(m_Strings.popupTitle);
	setContent(m_Editfield->getEvasObject());
	addButton(m_Strings.buttonCancel);

	Evas_Object *button = addButton(m_Strings.buttonDone, std::bind(&EditfieldPopup::onDoneButtonPressed, this));
	Evas_Object *entry = m_Editfield->getEntry();
	elm_entry_input_panel_return_key_autoenabled_set(entry, EINA_TRUE);
	elm_entry_input_panel_return_key_type_set(entry, ELM_INPUT_PANEL_RETURN_KEY_TYPE_DONE);

	evas_object_smart_callback_add(entry, "changed",
			(Evas_Smart_Cb) &EditfieldPopup::onEntryChanged, button);
	evas_object_smart_callback_add(entry, "activated",
			makeCallback(&EditfieldPopup::onDoneKeyPressed), this);

	elm_object_disabled_set(button, EINA_TRUE);
	elm_object_focus_set(entry, EINA_TRUE);
}

bool EditfieldPopup::onDoneButtonPressed()
{
	if (m_OnResult) {
		char *text = elm_entry_markup_to_utf8(elm_entry_entry_get(m_Editfield->getEntry()));
		m_OnResult(text);
		free(text);
	}

	return true;
}

void EditfieldPopup::onDoneKeyPressed(Evas_Object *entry, void *eventInfo)
{
	onDoneButtonPressed();
	delete this;
}

void EditfieldPopup::onEntryChanged(Evas_Object *button, Evas_Object *entry, void *eventInfo)
{
	elm_object_disabled_set(button, elm_entry_is_empty(entry));
}
