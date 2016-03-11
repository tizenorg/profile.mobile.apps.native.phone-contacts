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
#include "Utils/Callback.h"

using namespace Ui;

NaviframePage::NaviframePage(Elm_Object_Item *naviItem)
	: m_NaviItem(naviItem)
{
	elm_object_item_data_set(m_NaviItem, this);
	elm_object_item_del_cb_set(m_NaviItem, makeCallback(&NaviframePage::onDestroy));
}

void NaviframePage::setTitle(const char *title)
{
	bool isEnabled = title && *title;
	elm_object_item_translatable_part_text_set(m_NaviItem, "elm.text.title", title);
	elm_naviframe_item_title_enabled_set(m_NaviItem, isEnabled, EINA_TRUE);
}

void NaviframePage::setStyle(const char *style)
{
	elm_naviframe_item_style_set(m_NaviItem, style);
}

void NaviframePage::setContent(const char *part, Evas_Object *content)
{
	elm_object_item_part_content_set(m_NaviItem, part, content);
}

void NaviframePage::onDestroy(Evas_Object *obj, void *eventInfo)
{
	delete this;
}
