/*
 * Copyright (c) 2015-2016 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "Ui/ScrollNavigator.h"
#include "Ui/ScrollPage.h"
#include "Utils/Callback.h"
#include "Utils/Logger.h"

using namespace Ui;

ScrollNavigator::ScrollNavigator()
	: Navigator(TabNavigator),
	  m_Tabbar(nullptr), m_Scroller(nullptr), m_Box(nullptr),
	  m_Width(0), m_Height(0),
	  m_IsNavigating(false), m_IsNavigationEnabled(true),
	  m_CurrentPage(nullptr)
{
}

ScrollPage *ScrollNavigator::getCurrentPage() const
{
	return m_CurrentPage;
}

void ScrollNavigator::setNavigationEnabled(bool isEnabled)
{
	m_IsNavigationEnabled = isEnabled;
	if (isEnabled) {
		elm_toolbar_select_mode_set(m_Tabbar, ELM_OBJECT_SELECT_MODE_ALWAYS);
		elm_scroller_movement_block_set(getEvasObject(), ELM_SCROLLER_MOVEMENT_NO_BLOCK);
	} else {
		elm_toolbar_select_mode_set(m_Tabbar, ELM_OBJECT_SELECT_MODE_NONE);
		elm_scroller_movement_block_set(getEvasObject(), ELM_SCROLLER_MOVEMENT_BLOCK_HORIZONTAL);
	}
}

Evas_Object *ScrollNavigator::onCreate(Evas_Object *parent)
{
	m_Scroller = elm_scroller_add(parent);
	elm_scroller_page_relative_set(m_Scroller, 1.0, 0.0);
	elm_scroller_page_scroll_limit_set(m_Scroller, 1, 0);
	elm_scroller_loop_set(m_Scroller, EINA_FALSE, EINA_FALSE);
	elm_scroller_policy_set(m_Scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
	elm_object_scroll_lock_y_set(m_Scroller, EINA_TRUE);

	evas_object_event_callback_add(m_Scroller, EVAS_CALLBACK_RESIZE,
			makeCallback(&ScrollNavigator::onResize), this);
	evas_object_smart_callback_add(m_Scroller, "scroll,page,changed",
			makeCallback(&ScrollNavigator::onPageChanged), this);

	m_Box = elm_box_add(m_Scroller);
	elm_box_horizontal_set(m_Box, EINA_TRUE);
	elm_object_content_set(m_Scroller, m_Box);

	return m_Scroller;
}

void ScrollNavigator::onPageAttached(NavigatorPage *page)
{
	m_Tabbar = elm_toolbar_add(getEvasObject());
	elm_toolbar_shrink_mode_set(m_Tabbar, ELM_TOOLBAR_SHRINK_EXPAND);
	elm_toolbar_select_mode_set(m_Tabbar, ELM_OBJECT_SELECT_MODE_ALWAYS);
	elm_toolbar_transverse_expanded_set(m_Tabbar, EINA_TRUE);
	evas_object_smart_callback_add(m_Tabbar, "selected",
			(Evas_Smart_Cb) makeCallback(&ScrollNavigator::onTabSelected), this);

	page->setStyle("tabbar/notitle");
	page->setContent("tabbar", m_Tabbar);
}

ScrollPage *ScrollNavigator::attachView(View *view)
{
	ScrollPage *page = new ScrollPage(m_Pages.size());
	m_Pages.push_back(page);

	Elm_Object_Item *item = elm_toolbar_item_append(m_Tabbar, nullptr, nullptr, nullptr, page);
	page->onTabAttached(item);

	Evas_Object *layout = page->create(getEvasObject());
	page->setContent("elm.swallow.content", view->create(layout));
	evas_object_size_hint_min_set(page->m_Sizer, m_Width, m_Height);
	elm_box_pack_end(m_Box, layout);
	evas_object_show(layout);

	return page;
}

void ScrollNavigator::navigateToPage(NavigatorPage *page)
{
	if (!m_IsNavigationEnabled) {
		return;
	}

	m_IsNavigating = true;
	notifyNavigation(getCurrentPage(), false);

	size_t currentIndex = 0;
	elm_scroller_current_page_get(m_Scroller, (int *) &currentIndex, nullptr);

	ScrollPage *tabPage = static_cast<ScrollPage *>(page);
	if (currentIndex != tabPage->m_Index) {
		elm_scroller_page_show(m_Scroller, tabPage->m_Index, 0);
	}
	if (!elm_toolbar_item_selected_get(tabPage->getTabItem())) {
		elm_toolbar_item_selected_set(tabPage->getTabItem(), EINA_TRUE);
	}
	m_CurrentPage = tabPage;

	m_IsNavigating = false;
	notifyNavigation(tabPage, true);
}

void ScrollNavigator::navigateFromPage(NavigatorPage *page)
{
	size_t index = static_cast<ScrollPage *>(page)->m_Index;
	m_Pages.erase(m_Pages.begin() + index);

	for (; index < m_Pages.size(); ++index) {
		--(m_Pages[index]->m_Index);
	}

	delete page;
}

void ScrollNavigator::onTabSelected(Evas_Object *obj, Elm_Object_Item *selectedItem)
{
	if (!m_IsNavigating) {
		ScrollPage *page = (ScrollPage *) elm_object_item_data_get(selectedItem);
		navigateTo(page->getView());
	}
}

void ScrollNavigator::onPageChanged(Evas_Object *obj, void *eventInfo)
{
	if (!m_IsNavigating) {
		int index = 0;
		elm_scroller_current_page_get(m_Scroller, &index, nullptr);
		navigateTo(m_Pages[index]->getView());
	}
}

void ScrollNavigator::onResize(Evas *e, Evas_Object *obj, void *eventInfo)
{
	evas_object_geometry_get(obj, nullptr, nullptr, &m_Width, &m_Height);
	for (auto &&page : m_Pages) {
		evas_object_size_hint_min_set(page->m_Sizer, m_Width, m_Height);
	}

	if (m_CurrentPage) {
		m_IsNavigating = true;
		elm_scroller_page_show(m_Scroller, m_CurrentPage->m_Index, 0);
		m_IsNavigating = false;
	}
}
