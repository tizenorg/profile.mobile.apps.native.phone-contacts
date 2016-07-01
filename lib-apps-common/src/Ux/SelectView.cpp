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
#include <notification.h>

#define TITLE_BUFFER_SIZE 32
#define POPUP_BUFFER_SIZE 256

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
			m_IsChecking = true;
			for (size_t i = m_Items.size() - 1; m_SelectCount > m_SelectLimit; --i) {
				if (!m_Items[i]->isExcluded()) {
					m_Items[i]->setChecked(false);
				}
			}

			m_IsChecking = false;
		}

		updateSelectAllState();
		updateDoneButtonState();
		updatePageTitle();

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

size_t SelectView::getSelectMax() const
{
	if (m_SelectLimit && m_SelectLimit < m_TotalCount) {
		return m_SelectLimit;
	}

	return m_TotalCount;
}

size_t SelectView::getSelectCount() const
{
	return m_SelectCount;
}

void SelectView::addSelectItem(SelectItem *item)
{
	item->m_SelectView = this;
	item->setSelectMode(m_SelectMode);
	m_Items.push_back(item);

	if (!item->isExcluded()) {
		updateItemCount(CountIncrement, item);
	}
}

void SelectView::removeSelectItem(SelectItem *item)
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

void SelectView::onPageAttached(Ui::NavigatorPage *page)
{
	updatePageTitle();
	updatePageButtons();
}

bool SelectView::onBackPressed()
{
	if (m_SelectMode != SelectNone) {
		if (m_OnCanceled && !m_OnCanceled()) {
			return false;
		}
	}

	return true;
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
				title = m_SelectCount ? m_Strings.titleWithCount : m_Strings.titleMulti;
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
	if (m_SelectMode == SelectMulti && m_TotalCount) {
		if (!m_SelectAllItem) {
			m_SelectAllItem.reset(new SelectAllItem(m_Strings.selectAll));
			m_SelectAllItem->setCheckCallback(std::bind(&SelectView::onSelectAllChecked, this, _1));
			m_SelectAllItem->setDestroyCallback(std::bind(&SelectView::onSelectAllDestroy, this));
			onSelectAllInsert(m_SelectAllItem.get());
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
		m_SelectAllItem->setChecked(m_SelectCount == getSelectMax());
	}
}

void SelectView::updateSelectCount(CountChange change, SelectItem *item)
{
	/* CURRENT count if incremented, PREVIOUS count otherwise */
	size_t checkCount = (change == CountIncrement) ? ++m_SelectCount : m_SelectCount--;

	if (m_SelectLimit && m_SelectCount > m_SelectLimit) {
		item->setChecked(false);
		return;
	}

	/* Prevent updating if multiple checking is in progress */
	if (m_IsChecking) {
		return;
	}

	/* m_SelectCount: (all - 1) -> all or all -> (all - 1) */
	if (checkCount == getSelectMax()) {
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
	/* PREVIOUS count if incremented, CURRENT count otherwise */
	size_t checkCount = (change == CountIncrement) ? m_TotalCount++ : --m_TotalCount;

	if (item->isChecked()) {
		updateSelectCount(change, item);
	}

	/* (all checked -> unchecked inserted) or (one unchecked -> unchecked removed) */
	if (checkCount == m_SelectCount) {
		updateSelectAllState();
	}
	/* m_TotalCount: 0 -> 1 or 1 -> 0 */
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
		if (m_OnSelected && m_OnSelected({ result })) {
			getPage()->close();
		}
	}
}

bool SelectView::onItemChecked(SelectItem *item, bool isChecked)
{
	if (m_SelectLimit && m_SelectCount == m_SelectLimit && isChecked) {
		char buffer[POPUP_BUFFER_SIZE];
		snprintf(buffer, sizeof(buffer), _(m_Strings.popupLimit), m_SelectLimit);
		notification_status_message_post(buffer);
		return false;
	}

	if (m_OnChecked && !m_OnChecked(item, isChecked, m_IsChecking)) {
		return false;
	}

	updateSelectCount(isChecked ? CountIncrement : CountDecrement, item);
	return true;
}

bool SelectView::onSelectAllChecked(bool isChecked)
{
	if (isChecked == (m_SelectCount == getSelectMax())) {
		return true;
	}

	m_IsChecking = true;
	for (auto &&item : m_Items) {
		if (!item->isExcluded()) {
			if (!item->setChecked(isChecked)) {
				break;
			}
		}
	}

	m_IsChecking = false;
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

	if (m_OnSelected && m_OnSelected(std::move(results))) {
		getPage()->close();
	}
}

void SelectView::onCancelPressed(Evas_Object *button, void *eventInfo)
{
	if (!m_OnCanceled || m_OnCanceled()) {
		getPage()->close();
	}
}
