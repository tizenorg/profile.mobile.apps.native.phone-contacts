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

#include "Ui/TabView.h"
#include "Ui/TabPage.h"
#include "Utils/Callback.h"

using namespace Ui;

TabView::TabView()
	: Navigator(TabNavigator), m_Tabbar(nullptr), m_Scroller(nullptr), m_Box(nullptr),
	  m_Width(0), m_Height(0), m_IsNavigating(false), m_CurrentPage(0)
{
}

View *TabView::getCurrentView() const
{
	return m_CurrentPage < m_Views.size() ? m_Views[m_CurrentPage] : nullptr;
}

Evas_Object *TabView::onCreate(Evas_Object *parent)
{
	m_Scroller = elm_scroller_add(parent);
	elm_scroller_page_relative_set(m_Scroller, 1.0, 0.0);
	elm_scroller_page_scroll_limit_set(m_Scroller, 1, 0);
	elm_scroller_loop_set(m_Scroller, EINA_FALSE, EINA_FALSE);
	elm_scroller_policy_set(m_Scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
	elm_object_scroll_lock_y_set(m_Scroller, EINA_TRUE);

	evas_object_event_callback_add(m_Scroller, EVAS_CALLBACK_RESIZE,
			makeCallback(&TabView::onResize), this);
	evas_object_smart_callback_add(m_Scroller, "scroll,page,changed",
			makeCallback(&TabView::onPageChanged), this);

	m_Box = elm_box_add(m_Scroller);
	elm_box_horizontal_set(m_Box, EINA_TRUE);
	elm_object_content_set(m_Scroller, m_Box);

	return m_Scroller;
}

void TabView::onPageAttached()
{
	m_Tabbar = elm_toolbar_add(getEvasObject());
	elm_toolbar_shrink_mode_set(m_Tabbar, ELM_TOOLBAR_SHRINK_EXPAND);
	elm_toolbar_select_mode_set(m_Tabbar, ELM_OBJECT_SELECT_MODE_ALWAYS);
	elm_toolbar_transverse_expanded_set(m_Tabbar, EINA_TRUE);
	evas_object_smart_data_set(m_Tabbar, this);
	evas_object_smart_callback_add(m_Tabbar, "selected",
			makeCallback(&TabView::onTabSelect), this);

	getPage()->setStyle("tabbar/notitle");
	getPage()->setContent("tabbar", m_Tabbar);
}

NavigatorPage *TabView::attachView(View *view)
{
	Elm_Object_Item *item = elm_toolbar_item_append(m_Tabbar, nullptr, nullptr, nullptr, nullptr);
	elm_object_item_data_set(item, view);
	elm_object_item_del_cb_set(item, &TabView::onTabDestroy);
	m_Views.push_back(view);

	size_t index = m_Views.size() - 1;
	TabPage *page = new TabPage(index, item);

	Evas_Object *layout = page->create(getEvasObject());
	page->setContent("elm.swallow.content", view->create(layout));
	evas_object_size_hint_min_set(page->m_Sizer, m_Width, m_Height);
	elm_box_pack_end(m_Box, layout);
	evas_object_show(layout);

	return page;
}

void TabView::navigateToView(View *view)
{
	m_IsNavigating = true;

	TabPage *page = static_cast<TabPage *>(view->getPage());
	elm_scroller_page_show(m_Scroller, page->m_Index, 0);
	elm_toolbar_item_selected_set(page->m_TabItem, EINA_TRUE);
	m_CurrentPage = page->m_Index;

	m_IsNavigating = false;
}

void TabView::onTabDestroy(void *data, Evas_Object *obj, void *tabItem)
{
	TabView *tabView = (TabView *) evas_object_smart_data_get(obj);
	TabPage *page = static_cast<TabPage *>(((View *) data)->getPage());

	auto it = tabView->m_Views.begin() + page->m_Index;
	it = tabView->m_Views.erase(it);

	for (; it !=  tabView->m_Views.end(); ++it) {
		page = static_cast<TabPage *>((*it)->getPage());
		--page->m_Index;
	}
}

void TabView::onTabSelect(Evas_Object *obj, void *tabItem)
{
	View *view = (View *) elm_object_item_data_get((Elm_Object_Item *) tabItem);
	if (!m_IsNavigating) {
		navigateTo(view);
	}
}

void TabView::onPageChanged(Evas_Object *obj, void *eventInfo)
{
	int index = 0;
	elm_scroller_current_page_get(m_Scroller, &index, nullptr);
	if (!m_IsNavigating) {
		navigateTo(m_Views[index]);
	}
}

void TabView::onResize(Evas *e, Evas_Object *obj, void *eventInfo)
{
	evas_object_geometry_get(obj, nullptr, nullptr, &m_Width, &m_Height);

	for (auto &&view : m_Views) {
		TabPage *page = static_cast<TabPage *>(view->getPage());
		evas_object_size_hint_min_set(page->m_Sizer, m_Width, m_Height);
	}

	elm_scroller_page_size_set(m_Scroller, m_Width, m_Height);
	elm_scroller_page_show(m_Scroller, m_CurrentPage, 0);
}
