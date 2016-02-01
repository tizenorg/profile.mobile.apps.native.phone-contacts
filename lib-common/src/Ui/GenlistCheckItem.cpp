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

using namespace Ui;

GenlistCheckItem::GenlistCheckItem()
	: m_Checked(false)
{ }

bool GenlistCheckItem::isChecked() const
{
	return m_Checked;
}

void GenlistCheckItem::setChecked(bool state)
{
	m_Checked = state;
	elm_check_state_set(getCheck(), m_Checked);
}

void GenlistCheckItem::setSelectedCallback(SelectedCallback callback)
{
	m_OnSelected = std::move(callback);
}

void GenlistCheckItem::unsetSelectedCallback()
{
	m_OnSelected = nullptr;
}

Evas_Object *GenlistCheckItem::getContent(Evas_Object *parent, const char *part)
{
	m_CheckPart = part;
	return createCheck(parent);
}

void GenlistCheckItem::onSelected()
{
	toggleCheckState();

	if (m_OnSelected) {
		m_OnSelected();
	}
}

Evas_Object *GenlistCheckItem::createCheck(Evas_Object *parent)
{
	Elm_Check *check = elm_check_add(parent);
	elm_check_state_set(check, m_Checked);
	return check;
}

void GenlistCheckItem::toggleCheckState()
{
	m_Checked = !m_Checked;
	elm_check_state_set(getCheck(), m_Checked);
}

Evas_Object *GenlistCheckItem::getCheck() const
{
	return elm_object_item_part_content_get(getObjectItem(), m_CheckPart.c_str());
}
