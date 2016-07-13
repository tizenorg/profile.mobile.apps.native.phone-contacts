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

#include "Ux/DatePopup.h"

using namespace Ux;

DatePopup::DatePopup(const char *format, tm date)
	: m_Format(format ? format : ""), m_Date(date),
	  m_DatePicker(nullptr), m_Strings{ nullptr }
{
}

void DatePopup::setStrings(Strings strings)
{
	m_Strings = strings;
}

void DatePopup::setResultCallback(ResultCallback callback)
{
	m_OnResult = std::move(callback);
}

Evas_Object *DatePopup::getDatePicker() const
{
	return m_DatePicker;
}

void DatePopup::onCreated()
{
	setTitle(m_Strings.popupTitle);
	addButton(m_Strings.buttonCancel);
	addButton(m_Strings.buttonDone, std::bind(&DatePopup::onDonePressed, this));

	m_DatePicker = elm_datetime_add(getEvasObject());
	elm_datetime_format_set(m_DatePicker, m_Format.c_str());
	elm_datetime_value_set(m_DatePicker, &m_Date);
	setContent(m_DatePicker);
}

bool DatePopup::onDonePressed()
{
	if (m_OnResult) {
		elm_datetime_value_get(m_DatePicker, &m_Date);
		m_OnResult(m_Date);
	}

	return true;
}
