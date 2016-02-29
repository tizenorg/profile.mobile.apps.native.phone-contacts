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

#include "Contacts/Common/SelectView.h"
#include "Contacts/Common/SelectAllItem.h"
#include "Contacts/Common/SelectItem.h"

#include "Ui/Genlist.h"
#include "Utils/Callback.h"

#include <app_i18n.h>
#include <algorithm>

#define TITLE_BUFFER_SIZE 32

using namespace Contacts;
using namespace std::placeholders;

SelectView::SelectView()
	: m_SelectAllItem(nullptr),
	  m_DoneButton(nullptr), m_CancelButton(nullptr),
	  m_SelectCount(0), m_SelectLimit(0),
	  m_SelectMode(SelectNone)
{
}

SelectView::~SelectView()
{
	if (m_SelectAllItem && !m_SelectAllItem->isInserted()) {
		delete m_SelectAllItem;
	}
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
				if (m_Items[i]->isChecked()) {
					m_Items[i]->setChecked(false);
					--m_SelectCount;
				}
			}
		}

		updatePageTitle();
		updateSelectAllItem();
		onSelectLimitChanged(m_SelectLimit);
	}
}

void SelectView::setSelectCallback(SelectCallback callback)
{
	m_OnSelected = std::move(callback);
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

const char *SelectView::getPageTitle() const
{
	switch (m_SelectMode) {
		case SelectSingle:
			return "IDS_PB_HEADER_SELECT";
		case SelectMulti:
			if (m_SelectLimit) {
				return "%zu/%zu";
			} else {
				return "IDS_PB_HEADER_PD_SELECTED_ABB";
			}
		default:
			return "";
	}
}

void SelectView::onPageAttached()
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

	updateSelectAllItem();
}

void SelectView::onItemRemove(SelectItem *item)
{
	if (item->isChecked()) {
		--m_SelectCount;
		updatePageTitle();
	}

	m_Items.erase(std::remove(m_Items.begin(), m_Items.end(), item), m_Items.end());
	updateSelectAllItem();
}

void SelectView::updatePageTitle()
{
	Ui::NavigatorPage *page = getPage();
	if (!page) {
		return;
	}

	char title[TITLE_BUFFER_SIZE];
	snprintf(title, sizeof(title), _(getPageTitle()), m_SelectCount, m_SelectLimit);
	page->setTitle(title);
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
			destroyPageButtons();
			break;

		case SelectMulti:
			createPageButtons();
			page->setContent("title_right_btn", m_DoneButton);
			page->setContent("title_left_btn", m_CancelButton);
			break;
	}
}

void SelectView::updateSelectAllItem()
{
	if (m_SelectMode == SelectMulti && !m_SelectLimit && !m_Items.empty()) {
		if (!m_SelectAllItem) {
			m_SelectAllItem = new SelectAllItem();
			m_SelectAllItem->setCheckCallback(std::bind(&SelectView::onSelectAllChecked, this, _1));
			onSelectAllInsert(m_SelectAllItem);
		}

		m_SelectAllItem->setChecked(m_SelectCount == m_Items.size());
	} else {
		onSelectAllRemove();
		delete m_SelectAllItem;
		m_SelectAllItem = nullptr;
	}
}

void SelectView::createPageButtons()
{
	m_DoneButton = elm_button_add(getEvasObject());
	elm_object_style_set(m_DoneButton, "naviframe/title_right");
	elm_object_translatable_text_set(m_DoneButton, "IDS_PB_BUTTON_DONE_ABB3");
	evas_object_smart_callback_add(m_DoneButton, "clicked",
			makeCallback(&SelectView::onDonePressed), this);

	m_CancelButton = elm_button_add(getEvasObject());
	elm_object_style_set(m_CancelButton, "naviframe/title_left");
	elm_object_translatable_text_set(m_CancelButton, "IDS_PB_BUTTON_CANCEL");
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

void SelectView::onItemSelected(SelectItem *item)
{
	switch (m_SelectMode) {
		case SelectNone:
			onItemPressed(item);
			break;
		case SelectSingle:
		{
			SelectResult result = item->getSelectResult();
			if (m_OnSelected && m_OnSelected({ &result, 1 })) {
				delete this;
			}
		}
			break;
		default:
			break;
	}
}

void SelectView::onItemChecked(SelectItem *item, bool isChecked)
{
	if (m_SelectLimit && m_SelectCount == m_SelectLimit && isChecked) {
		item->setChecked(false);
	} else {
		size_t checkCount = isChecked ? ++m_SelectCount : m_SelectCount--;
		if (checkCount == m_Items.size()) {
			updateSelectAllItem();
		}

		updatePageTitle();
	}
}

void SelectView::onSelectAllChecked(bool isChecked)
{
	for (auto &&item : m_Items) {
		item->setChecked(isChecked);
	}

	m_SelectCount = isChecked ? m_Items.size() : 0;
	updatePageTitle();
}

void SelectView::onDonePressed(Evas_Object *button, void *eventInfo)
{
	std::vector<SelectResult> results;
	for (auto &&item : m_Items) {
		if (item->isChecked()) {
			results.push_back(item->getSelectResult());
		}
	}

	if (m_OnSelected && m_OnSelected({ results.data(), results.size()})) {
		delete this;
	}
}

void SelectView::onCancelPressed(Evas_Object *button, void *eventInfo)
{
	delete this;
}
