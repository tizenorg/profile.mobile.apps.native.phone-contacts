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

#include "Ux/SelectView.h"
#include "Ux/SelectItem.h"

#include "Ui/Genlist.h"
#include "Utils/Callback.h"

#include <app_i18n.h>
#include <algorithm>

#define TITLE_BUFFER_SIZE 32

using namespace Ux;
using namespace std::placeholders;

SelectView::SelectView()
	: m_DoneButton(nullptr), m_CancelButton(nullptr), m_IsChecking(false),
	  m_TotalCount(0), m_SelectCount(0), m_SelectLimit(0), m_SelectMode(SelectNone),
	  m_Strings{ nullptr }
{
}

void SelectView::setStrings(const Strings &strings)
{
	m_Strings = strings;
}

void SelectView::setSelectMode(SelectMode selectMode)
{
	if (m_SelectMode != selectMode) {
		m_SelectMode = selectMode;

		updatePageTitle();
		updatePageButtons();
		updateSelectAllItem();

		for (auto &&item : m_Items) {
			item->setSelectMode(m_SelectMode);
		}

		onSelectModeChanged(m_SelectMode);
	}
}

void SelectView::setSelectLimit(size_t selectLimit)
{
	if (m_SelectLimit != selectLimit) {
		m_SelectLimit = selectLimit;

		if (m_SelectLimit) {
			for (size_t i = m_Items.size() - 1; m_SelectCount > m_SelectLimit; --i) {
				if (!m_Items[i]->isExcluded() && m_Items[i]->isChecked()) {
					m_Items[i]->setChecked(false);
				}
			}
		}

		onSelectLimitChanged(m_SelectLimit);
	}
}

void SelectView::setSelectCallback(SelectCallback callback)
{
	m_OnSelected = std::move(callback);
}

void SelectView::setCancelCallback(CancelCallback callback)
{
	m_OnCanceled = std::move(callback);
}

void SelectView::setCheckCallback(CheckCallback callback)
{
	m_OnChecked = std::move(callback);
}

SelectMode SelectView::getSelectMode() const
{
	return m_SelectMode;
}

size_t SelectView::getSelectLimit() const
{
	return m_SelectLimit;
}

size_t SelectView::getSelectCount() const
{
	return m_SelectCount;
}

void SelectView::onPageAttached(Ui::NavigatorPage *page)
{
	updatePageTitle();
	updatePageButtons();
}

void SelectView::onItemInserted(SelectItem *item)
{
	item->setSelectMode(m_SelectMode);
	item->setSelectCallback(std::bind(&SelectView::onItemSelected, this, item));
	item->setCheckCallback(std::bind(&SelectView::onItemChecked, this, item, _1));
	m_Items.push_back(item);

	if (!item->isExcluded()) {
		updateItemCount(CountIncrement, item);
	}
	item->m_SelectView = this;
}

void SelectView::onItemRemove(SelectItem *item)
{
	auto it = std::find(m_Items.begin(), m_Items.end(), item);
	if (it != m_Items.end()) {
		m_Items.erase(it);
	}

	if (!item->isExcluded()) {
		updateItemCount(CountDecrement, item);
	}
	item->m_SelectView = nullptr;
}

void SelectView::updatePageTitle()
{
	Ui::NavigatorPage *page = getPage();
	if (!page) {
		return;
	}

	const char *title = nullptr;
	switch (m_SelectMode) {
		case SelectNone:
			title = m_Strings.titleDefault;
			break;
		case SelectSingle:
			title = m_Strings.titleSingle;
			break;
		case SelectMulti:
			if (m_SelectLimit) {
				title = m_Strings.titleWithLimit;
			} else {
				title = m_Strings.titleMulti;
			}
			break;
	}

	char buffer[TITLE_BUFFER_SIZE];
	snprintf(buffer, sizeof(buffer), _(title), m_SelectCount, m_SelectLimit);
	page->setTitle(buffer);
}

void SelectView::updatePageButtons()
{
	Ui::NavigatorPage *page = getPage();
	if (!page) {
		return;
	}

	switch (m_SelectMode) {
		case SelectNone:
		case SelectSingle:
			if (m_DoneButton) {
				destroyPageButtons();
			}
			break;

		case SelectMulti:
			if (!m_DoneButton) {
				createPageButtons();
				page->setContent("title_right_btn", m_DoneButton);
				page->setContent("title_left_btn", m_CancelButton);
			}
			updateDoneButtonState();
			break;
	}
}

void SelectView::updateSelectAllItem()
{
	if (m_SelectMode == SelectMulti && !m_SelectLimit && m_TotalCount) {
		if (!m_SelectAllItem) {
			m_SelectAllItem.reset(new SelectAllItem(m_Strings.selectAll));
			m_SelectAllItem->setCheckCallback(std::bind(&SelectView::onSelectAllChecked, this, _1));
			m_SelectAllItem->setDestroyCallback(std::bind(&SelectView::onSelectAllDestroy, this));
			onSelectAllInsert(m_SelectAllItem.get());
			m_SelectAllItem->scrollTo();
		}

		updateSelectAllState();
	} else {
		if (m_SelectAllItem) {
			onSelectAllRemove();
			m_SelectAllItem.reset();
		}
	}
}

void SelectView::updateDoneButtonState()
{
	elm_object_disabled_set(m_DoneButton, m_SelectCount == 0);
}

void SelectView::updateSelectAllState()
{
	if (m_SelectAllItem) {
		m_SelectAllItem->setChecked(m_SelectCount == m_TotalCount);
	}
}

void SelectView::updateSelectCount(CountChange change)
{
	/* CURRENT count if incremented, PREVIOUS count otherwise */
	size_t checkCount = (change == CountIncrement) ? ++m_SelectCount : m_SelectCount--;

	/* m_SelectCount: (all - 1) -> all or all -> (all - 1) */
	if (checkCount == m_TotalCount) {
		updateSelectAllState();
	}
	/* m_SelectCount: 0 -> 1 or 1 -> 0 */
	if (checkCount == 1) {
		updateDoneButtonState();
	}

	updatePageTitle();
}

void SelectView::updateItemCount(CountChange change, SelectItem *item)
{
	if (item->isChecked()) {
		if (change == CountIncrement && m_SelectLimit && m_SelectCount == m_SelectLimit) {
			item->setChecked(false);
		} else {
			updateSelectCount(change);
		}
	}

	/* PREVIOUS count if incremented, CURRENT count otherwise */
	size_t checkCount = (change == CountIncrement) ? m_TotalCount++ : --m_TotalCount;

	/* (all checked -> unchecked inserted) or (one unchecked -> unchecked removed) */
	if (checkCount == m_SelectCount) {
		updateSelectAllState();
	}
	/* m_Items.size(): 0 -> 1 or 1 -> 0 */
	if (checkCount == 0) {
		updateSelectAllItem();
	}
}

void SelectView::createPageButtons()
{
	m_DoneButton = elm_button_add(getEvasObject());
	elm_object_style_set(m_DoneButton, "naviframe/title_right");
	elm_object_translatable_text_set(m_DoneButton, m_Strings.buttonDone);
	evas_object_smart_callback_add(m_DoneButton, "clicked",
			makeCallback(&SelectView::onDonePressed), this);

	m_CancelButton = elm_button_add(getEvasObject());
	elm_object_style_set(m_CancelButton, "naviframe/title_left");
	elm_object_translatable_text_set(m_CancelButton, m_Strings.buttonCancel);
	evas_object_smart_callback_add(m_CancelButton, "clicked",
			makeCallback(&SelectView::onCancelPressed), this);
}

void SelectView::destroyPageButtons()
{
	evas_object_del(m_DoneButton);
	evas_object_del(m_CancelButton);

	m_DoneButton = nullptr;
	m_CancelButton = nullptr;
}

void SelectView::onItemExcluded(SelectItem *item, bool isExcluded)
{
	updateItemCount(isExcluded ? CountDecrement : CountIncrement, item);
}

void SelectView::onItemSelected(SelectItem *item)
{
	if  (m_SelectMode == SelectSingle) {
		SelectResult result = item->getSelectResult();
		if (m_OnSelected && m_OnSelected({ &result, 1 })) {
			getPage()->close();
		}
	}
}

bool SelectView::onItemChecked(SelectItem *item, bool isChecked)
{
	if (m_IsChecking || item->isExcluded()) {
		return true;
	}

	if (m_SelectLimit && m_SelectCount == m_SelectLimit && isChecked) {
		return false;
	}

	if (m_OnChecked && !m_OnChecked(item, isChecked)) {
		return false;
	}

	updateSelectCount(isChecked ? CountIncrement : CountDecrement);
	return true;
}

bool SelectView::onSelectAllChecked(bool isChecked)
{
	if (isChecked == (m_SelectCount == m_TotalCount)) {
		return true;
	}

	m_IsChecking = true;
	for (auto &&item : m_Items) {
		if (!item->isExcluded()) {
			item->setChecked(isChecked);
		}
	}

	m_IsChecking = false;
	m_SelectCount = isChecked ? m_TotalCount : 0;
	updatePageTitle();
	updateDoneButtonState();
	return true;
}

void SelectView::onSelectAllDestroy()
{
	m_SelectAllItem.release();
}

void SelectView::onDonePressed(Evas_Object *button, void *eventInfo)
{
	std::vector<SelectResult> results;
	for (auto &&item : m_Items) {
		if (!item->isExcluded() && item->isChecked()) {
			results.push_back(item->getSelectResult());
		}
	}

	if (m_OnSelected && m_OnSelected({ results.data(), results.size()})) {
		getPage()->close();
	}
}

void SelectView::onCancelPressed(Evas_Object *button, void *eventInfo)
{
	if (!m_OnCanceled || m_OnCanceled()) {
		getPage()->close();
	}
}
