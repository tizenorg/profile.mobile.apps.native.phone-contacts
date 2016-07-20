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

#include "Ui/ScrollPage.h"
#include "Ui/ScrollNavigator.h"

using namespace Ui;

ScrollPage::ScrollPage(size_t index)
	: m_Index(index), m_Sizer(nullptr)
{
}

Evas_Object *ScrollPage::onCreate(Evas_Object *parent)
{
	m_Sizer = evas_object_rectangle_add(evas_object_evas_get(parent));
	evas_object_color_set(m_Sizer, 0, 0, 0, 0);

	Evas_Object *layout = elm_layout_add(parent);
	elm_layout_theme_set(layout, "layout", "application", "default");
	elm_object_part_content_set(layout, "elm.swallow.bg", m_Sizer);
	elm_object_part_content_set(layout, "elm.swallow.content", TabPage::onCreate(layout));

	return layout;
}

void ScrollPage::setExpanded(bool isExpanded)
{
	auto navigator = static_cast<ScrollNavigator *>(getNavigator());
	navigator->setNavigationEnabled(!isExpanded);
	NavigatorPage::setExpanded(isExpanded);
}
