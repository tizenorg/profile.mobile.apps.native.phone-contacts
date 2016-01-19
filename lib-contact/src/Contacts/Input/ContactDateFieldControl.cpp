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

#include "Contacts/Input/ContactDateFieldControl.h"
#include "Contacts/Model/ContactDateField.h"

#include "Ui/DatePopup.h"
#include "Utils/Callback.h"

#include "CommonButtons.h"

#define DATE_BUFFER_SIZE 32

using namespace Contacts::Input;
using namespace Contacts::Model;

ContactDateFieldControl::ContactDateFieldControl(Model::ContactDateField &field)
	: m_Field(field), m_Button(nullptr)
{
}

void ContactDateFieldControl::update()
{
	tm date = m_Field.getValue();
	char buffer[DATE_BUFFER_SIZE];
	strftime(buffer, sizeof(buffer), "%x", &date);
	elm_object_text_set(m_Button, buffer);
}

Evas_Object *ContactDateFieldControl::onCreate(Evas_Object *parent)
{
	Evas_Object *box = elm_box_add(parent);

	m_Button = elm_button_add(box);
	elm_object_style_set(m_Button, BUTTON_STYLE_CUSTOM_DEFAULT);
	evas_object_smart_callback_add(m_Button, "clicked",
			makeCallback(&ContactDateFieldControl::onButtonPressed), this);

	evas_object_size_hint_weight_set(m_Button, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(m_Button, 0.5, 0.5);
	elm_box_pack_end(box, m_Button);
	evas_object_show(m_Button);

	return box;
}

void ContactDateFieldControl::onCreated()
{
	update();
}

void ContactDateFieldControl::onButtonPressed(Evas_Object *button, void *eventInfo)
{
	Ui::DatePopup *popup = new Ui::DatePopup(m_Field.getValue());
	popup->setResultCallback([this](const tm &date) {
		m_Field.setValue(date);
		update();
	});
	popup->create(getEvasObject());
}
