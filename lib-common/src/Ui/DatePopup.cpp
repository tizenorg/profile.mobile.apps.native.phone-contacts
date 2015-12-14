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

#include "Ui/DatePopup.h"

using namespace Ui;

DatePopup::DatePopup(tm date)
	: m_Date(date), m_DatePicker(nullptr)
{
}

void DatePopup::setResultCallback(ResultCallback callback)
{
	m_OnResult = std::move(callback);
}

void DatePopup::onCreated()
{
	setTitle("IDS_ST_BODY_SET_DATE");
	addButton("IDS_PB_BUTTON_CANCEL");
	addButton("IDS_PB_SK_SET", [this] {
		onSetPressed();
		return true;
	});

	m_DatePicker = elm_datetime_add(getEvasObject());
	elm_datetime_format_set(m_DatePicker, "%%x");
	elm_datetime_value_set(m_DatePicker, &m_Date);
	setContent(m_DatePicker);
}

void DatePopup::onSetPressed()
{
	if (m_OnResult) {
		elm_datetime_value_get(m_DatePicker, &m_Date);
		m_OnResult(m_Date);
	}
}
