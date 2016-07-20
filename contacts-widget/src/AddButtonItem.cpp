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

#include "AddButtonItem.h"
#include "WidgetLayout.h"
#include "WidgetItemLayout.h"

#include "App/Path.h"

namespace
{
	const std::string layoutPath = App::getResourcePath(WIDGET_LAYOUT_EDJ);
}

AddButtonItem::AddButtonItem()
	: GenItem(Ui::GenContainer::TypeGengrid)
{
}

Elm_Gengrid_Item_Class *AddButtonItem::getItemClass() const
{
	static Elm_Gengrid_Item_Class itc = createItemClass(WIDGET_ITEM_STYLE);
	return &itc;
}

Evas_Object *AddButtonItem::getContent(Evas_Object *parent, const char *part)
{
	if (strcmp(part, PART_THUMBNAIL) == 0) {
		Evas_Object *layout = elm_layout_add(parent);
		elm_layout_file_set(layout, layoutPath.c_str(), GROUP_ICON_ADD);
		return layout;
	}

	return nullptr;
}
