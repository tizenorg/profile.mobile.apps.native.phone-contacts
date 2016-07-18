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

#include "Ui/CheckItem.h"
#include "Utils/Callback.h"

using namespace Ui;

CheckItem::CheckItem()
	: m_CheckPart("*"), m_IsChecked(false), m_IsChecking(false),
	  m_LinkedItem(nullptr)
{
}

CheckItem::~CheckItem()
{
	unsetLinkedItem();
}

bool CheckItem::isChecked() const
{
	return m_IsChecked;
}

bool CheckItem::setChecked(bool isChecked)
{
	if (isChecked == m_IsChecked) {
		return true;
	}

	m_IsChecked = isChecked;
	if (!notifyCheck()) {
		m_IsChecked = !m_IsChecked;
		return false;
	}

	Evas_Object *check = elm_object_item_part_content_get(getObjectItem(), m_CheckPart.c_str());
	if (check) {
		elm_check_state_set(check, m_IsChecked);
	}

	return true;
}

void CheckItem::setCheckCallback(CheckCallback callback)
{
	m_OnChecked = std::move(callback);
}

void CheckItem::setLinkedItem(CheckItem *item)
{
	if (!item) {
		return;
	}

	unsetLinkedItem();
	item->unsetLinkedItem();

	item->setChecked(m_IsChecked);
	item->m_LinkedItem = this;
	m_LinkedItem = item;
}

void CheckItem::unsetLinkedItem()
{
	if (m_LinkedItem) {
		m_LinkedItem->m_LinkedItem = nullptr;
		m_LinkedItem = nullptr;
	}
}

void CheckItem::updateCheckPart()
{
	elm_genlist_item_fields_update(getObjectItem(), m_CheckPart.c_str(), ELM_GENLIST_ITEM_FIELD_CONTENT);
}

Evas_Object *CheckItem::getContent(Evas_Object *parent, const char *part)
{
	m_CheckPart = part;

	Elm_Check *check = elm_check_add(parent);
	elm_check_state_set(check, m_IsChecked);
	elm_check_state_pointer_set(check, &m_IsChecked);
	evas_object_propagate_events_set(check, EINA_FALSE);
	evas_object_smart_callback_add(check, "changed",
			makeCallback(&CheckItem::onCheckChanged), this);

	return check;
}

void CheckItem::onSelected()
{
	setChecked(!m_IsChecked);
}

void CheckItem::onCheckChanged(Evas_Object *check, void *eventInfo)
{
	if (!notifyCheck()) {
		elm_check_state_set(check, !m_IsChecked);
	}
}

bool CheckItem::notifyCheck()
{
	if (m_IsChecking) {
		return false;
	}

	bool isAllowed = false;
	m_IsChecking = true;

	if (onChecked(m_IsChecked)) {
		if (!m_OnChecked || m_OnChecked(m_IsChecked)) {
			if (!m_LinkedItem || m_LinkedItem->setChecked(m_IsChecked)) {
				isAllowed = true;
			}
		}
	}

	m_IsChecking = false;
	return isAllowed;
}
