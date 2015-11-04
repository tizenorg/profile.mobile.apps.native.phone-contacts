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

#include "Ui/NaviframePage.h"
using namespace Ui;

NaviframePage::NaviframePage(Elm_Object_Item *naviItem)
	: m_NaviItem(naviItem)
{
}

NaviframePage::~NaviframePage()
{
	elm_object_item_part_content_unset(m_NaviItem, "elm.swallow.content");
	Evas_Object *navi = elm_object_item_widget_get(m_NaviItem);
	if (m_NaviItem == elm_naviframe_top_item_get(navi)) {
		elm_naviframe_item_pop(navi);
	} else {
		elm_object_item_del(m_NaviItem);
	}
}

void NaviframePage::setTitle(const char *title)
{
	elm_object_item_domain_translatable_part_text_set(m_NaviItem, "elm.text.title", nullptr, title);
	elm_naviframe_item_title_enabled_set(m_NaviItem, title != nullptr, EINA_TRUE);
}

void NaviframePage::setStyle(const char *style)
{
	elm_naviframe_item_style_set(m_NaviItem, style);
}

void NaviframePage::setContent(const char *part, Evas_Object *content)
{
	elm_object_item_part_content_set(m_NaviItem, part, content);
}
