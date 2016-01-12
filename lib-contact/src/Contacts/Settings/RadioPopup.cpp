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

#include "Contacts/Settings/RadioPopup.h"

using namespace Contacts::Settings;

RadioPopup::RadioPopup()
	: m_RadioGroup(nullptr)
{
}

void RadioPopup::setSelectedItem(int value)
{
	elm_radio_value_set(m_RadioGroup, value);
}

Evas_Object *RadioPopup::onCreate(Evas_Object *parent)
{
	Evas_Object *popup = ListPopup::onCreate(parent);

	m_RadioGroup = elm_radio_add(popup);
	elm_radio_state_value_set(m_RadioGroup, -1);

	return popup;
}

Evas_Object *RadioPopup::getItemContent(void *data, const char *part)
{
	if (strcmp(part, "elm.swallow.end") == 0) {
		Evas_Object *radio = elm_radio_add(getEvasObject());
		elm_radio_group_add(radio, m_RadioGroup);
		elm_radio_state_value_set(radio, (long) data);
		return radio;
	}

	return nullptr;
}

void RadioPopup::onItemSelected(void *data)
{
	setSelectedItem((long) data);
}
