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

#include "Ux/SelectItem.h"
#include "Ux/SelectView.h"

using namespace Ux;

SelectItem::SelectItem()
	: m_SelectView(nullptr), m_SelectMode(SelectNone),
	  m_CustomResult{ 0, 0 }, m_HasCustomResult(false),
	  m_IsExcluded(false)
{
}

bool SelectItem::isExcluded() const
{
	return m_IsExcluded;
}

void SelectItem::setExcluded(bool isExcluded)
{
	if (m_IsExcluded == isExcluded) {
		return;
	}

	m_IsExcluded = isExcluded;
	if (m_SelectView) {
		m_SelectView->onItemExcluded(this, m_IsExcluded);
	}
}

SelectMode SelectItem::getSelectMode() const
{
	return m_SelectMode;
}

void SelectItem::setSelectMode(SelectMode selectMode)
{
	m_SelectMode = selectMode;

	setChecked(false);
	updateCheckPart();
	onSelectModeChanged(m_SelectMode);
}

SelectResult SelectItem::getSelectResult() const
{
	return m_HasCustomResult ? m_CustomResult : getDefaultResult();
}

bool SelectItem::hasCustomResult() const
{
	return m_HasCustomResult;
}

void SelectItem::setCustomResult(SelectResult result)
{
	m_CustomResult = result;
	m_HasCustomResult = true;
}

void SelectItem::unsetCustomResult()
{
	m_HasCustomResult = false;
}

Evas_Object *SelectItem::getContent(Evas_Object *parent, const char *part)
{
	if (m_SelectMode == SelectMulti) {
		return GenlistCheckItem::getContent(parent, part);
	}

	return nullptr;
}

void SelectItem::onSelected()
{
	if (m_SelectMode == SelectSingle) {
		if (m_SelectView) {
			m_SelectView->onItemSelected(this);
		}
	} else if (m_SelectMode == SelectMulti) {
		GenlistCheckItem::onSelected();
	}
}

bool SelectItem::onChecked(bool isChecked)
{
	if (m_SelectView && !m_IsExcluded) {
		return m_SelectView->onItemChecked(this, isChecked);
	}

	return true;
}
