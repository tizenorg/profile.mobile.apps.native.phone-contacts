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

#include "Ui/TabPage.h"
#include "Utils/Logger.h"

#define BUFFER_SIZE 64

using namespace Ui;

TabPage::TabPage(size_t index, Elm_Object_Item *tabItem)
	: m_Index(index), m_TabItem(tabItem), m_Sizer(nullptr), m_Page(nullptr)
{
}

TabPage::~TabPage()
{
	elm_object_part_content_unset(m_Page, "elm.swallow.content");
	elm_object_item_del(m_TabItem);
}

Evas_Object *TabPage::onCreate(Evas_Object *parent)
{
	Evas_Object *layout = elm_layout_add(parent);
	elm_layout_theme_set(layout, "layout", "application", "default");

	m_Sizer = evas_object_rectangle_add(evas_object_evas_get(layout));
	evas_object_color_set(m_Sizer, 0, 0, 0, 0);
	elm_object_part_content_set(layout, "elm.swallow.bg", m_Sizer);

	m_Page = elm_layout_add(parent);
	elm_layout_theme_set(m_Page, "naviframe", "item", "basic/default");
	elm_layout_signal_emit(m_Page, "elm,state,title,hide", "elm");
	elm_layout_signal_emit(m_Page, "elm,state,toolbar,hide", "elm");
	elm_object_part_content_set(layout, "elm.swallow.content", m_Page);

	return layout;
}

void TabPage::setTitle(const char *title)
{
	elm_object_item_translatable_text_set(m_TabItem, title);
}

void TabPage::setStyle(const char *style)
{
	char buffer[BUFFER_SIZE];
	snprintf(buffer, sizeof(buffer), "%s/default", style);
	elm_layout_theme_set(m_Page, "naviframe", "item", buffer);
}

void TabPage::setContent(const char *part, Evas_Object *content)
{
	char buffer[BUFFER_SIZE];
	snprintf(buffer, sizeof(buffer), "elm,state,%s,%s", part, content ? "show" : "hide");
	elm_layout_signal_emit(m_Page, buffer, "elm");
	elm_object_part_content_set(m_Page, part, content);
}
