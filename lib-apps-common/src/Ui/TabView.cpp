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
#include "Utils/Logger.h"

using namespace Ui;

TabView::TabView()
	: Navigator(TabNavigator),
	  m_Tabbar(nullptr), m_Scroller(nullptr), m_Box(nullptr),
	  m_Width(0), m_Height(0), m_IsNavigating(false), m_CurrentPage(nullptr)
{
}

TabPage *TabView::getCurrentPage() const
{
	return m_CurrentPage;
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

void TabView::onPageAttached(NavigatorPage *page)
{
	m_Tabbar = elm_toolbar_add(getEvasObject());
	elm_toolbar_shrink_mode_set(m_Tabbar, ELM_TOOLBAR_SHRINK_EXPAND);
	elm_toolbar_select_mode_set(m_Tabbar, ELM_OBJECT_SELECT_MODE_ALWAYS);
	elm_toolbar_transverse_expanded_set(m_Tabbar, EINA_TRUE);
	evas_object_smart_data_set(m_Tabbar, this);
	evas_object_smart_callback_add(m_Tabbar, "selected",
			(Evas_Smart_Cb) makeCallback(&TabView::onTabSelected), this);

	page->setStyle("tabbar/notitle");
	page->setContent("tabbar", m_Tabbar);
}

TabPage *TabView::attachView(View *view)
{
	TabPage *page = new TabPage(m_Pages.size());
	m_Pages.push_back(page);

	Elm_Object_Item *item = elm_toolbar_item_append(m_Tabbar, nullptr, nullptr, nullptr, page);
	elm_object_item_del_cb_set(item, &TabView::onTabDestroy);
	page->onTabAttached(item);

	Evas_Object *layout = page->create(getEvasObject());
	page->setContent("elm.swallow.content", view->create(layout));
	evas_object_size_hint_min_set(page->m_Sizer, m_Width, m_Height);
	elm_box_pack_end(m_Box, layout);
	evas_object_show(layout);

	return page;
}

void TabView::navigateToPage(NavigatorPage *page)
{
	m_IsNavigating = true;
	notifyNavigation(getCurrentPage(), false);

	TabPage *tabPage = static_cast<TabPage *>(page);
	elm_scroller_page_show(m_Scroller, tabPage->m_Index, 0);
	elm_toolbar_item_selected_set(tabPage->m_TabItem, EINA_TRUE);
	m_CurrentPage = tabPage;

	m_IsNavigating = false;
	notifyNavigation(tabPage, true);
}

void TabView::onTabDestroy(void *data, Evas_Object *obj, void *tabItem)
{
	TabView *tabView = (TabView *) evas_object_smart_data_get(obj);
	TabPage *page = (TabPage *) data;

	size_t index = page->m_Index;
	tabView->m_Pages.erase(tabView->m_Pages.begin() + index);

	for (; index < tabView->m_Pages.size(); ++index) {
		--(tabView->m_Pages[index]->m_Index);
	}
}

void TabView::onTabSelected(Evas_Object *obj, Elm_Object_Item *selectedItem)
{
	TabPage *page = (TabPage *) elm_object_item_data_get(selectedItem);
	if (!m_IsNavigating) {
		navigateTo(page->getView());
	}
}

void TabView::onPageChanged(Evas_Object *obj, void *eventInfo)
{
	int index = 0;
	elm_scroller_current_page_get(m_Scroller, &index, nullptr);
	if (!m_IsNavigating) {
		navigateTo(m_Pages[index]->getView());
	}
}

void TabView::onResize(Evas *e, Evas_Object *obj, void *eventInfo)
{
	evas_object_geometry_get(obj, nullptr, nullptr, &m_Width, &m_Height);
	for (auto &&page : m_Pages) {
		evas_object_size_hint_min_set(page->m_Sizer, m_Width, m_Height);
	}

	/* FIXME: elm_scroller_page_show() changes page to 0 regardless what index was passed */
	ecore_job_add([](void *data) {
		TabView *tabView = (TabView *) data;
		if (tabView->m_CurrentPage) {
			elm_scroller_page_show(tabView->m_Scroller, tabView->m_CurrentPage->m_Index, 0);
		}
	}, this);
}
