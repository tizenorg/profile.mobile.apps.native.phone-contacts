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

#include "Ui/GenlistCheckItem.h"
#include "Utils/Callback.h"

using namespace Ui;

GenlistCheckItem::GenlistCheckItem()
	: m_CheckPart("*"), m_IsChecked(false)
{ }

bool GenlistCheckItem::isChecked() const
{
	return m_IsChecked;
}

void GenlistCheckItem::setChecked(bool isChecked)
{
	Evas_Object *check = elm_object_item_part_content_get(getObjectItem(), m_CheckPart.c_str());
	if (check) {
		elm_check_state_set(check, isChecked);
	} else {
		m_IsChecked = isChecked;
	}
}

void GenlistCheckItem::setCheckCallback(CheckCallback callback)
{
	m_OnChecked = std::move(callback);
}

void GenlistCheckItem::updateCheckPart()
{
	elm_genlist_item_fields_update(getObjectItem(), m_CheckPart.c_str(), ELM_GENLIST_ITEM_FIELD_CONTENT);
}

Evas_Object *GenlistCheckItem::getContent(Evas_Object *parent, const char *part)
{
	m_CheckPart = part;

	Elm_Check *check = elm_check_add(parent);
	elm_check_state_set(check, m_IsChecked);
	elm_check_state_pointer_set(check, &m_IsChecked);
	evas_object_propagate_events_set(check, EINA_FALSE);
	evas_object_smart_callback_add(check, "changed",
			makeCallback(&GenlistCheckItem::onCheckChanged), this);

	return check;
}

void GenlistCheckItem::onSelected()
{
	setChecked(!m_IsChecked);
	onChecked();
}

void GenlistCheckItem::onChecked()
{
	onChecked(m_IsChecked);

	if (m_OnChecked) {
		m_OnChecked(m_IsChecked);
	}
}

void GenlistCheckItem::onCheckChanged(Evas_Object *check, void *eventInfo)
{
	onChecked();
}
