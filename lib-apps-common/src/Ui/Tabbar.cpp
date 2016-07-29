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

#include "Ui/Tabbar.h"
#include "Utils/Callback.h"

using namespace Ui;

Elm_Object_Item *Tabbar::addItem(void *data)
{
	return elm_toolbar_item_append(getEvasObject(), nullptr, nullptr, nullptr, data);
}

void Tabbar::setSelectEnabled(bool isEnabled)
{
	elm_toolbar_select_mode_set(getEvasObject(),
			isEnabled ? ELM_OBJECT_SELECT_MODE_ALWAYS : ELM_OBJECT_SELECT_MODE_NONE);
}

void Tabbar::setSelectedItem(Elm_Object_Item *item)
{
	elm_toolbar_item_selected_set(item, EINA_TRUE);
}

Evas_Object *Tabbar::onCreate(Evas_Object *parent)
{
	Evas_Object *tabbar = elm_toolbar_add(parent);
	elm_toolbar_shrink_mode_set(tabbar, ELM_TOOLBAR_SHRINK_EXPAND);
	elm_toolbar_select_mode_set(tabbar, ELM_OBJECT_SELECT_MODE_ALWAYS);
	elm_toolbar_transverse_expanded_set(tabbar, EINA_TRUE);
	evas_object_smart_callback_add(tabbar, "selected",
			(Evas_Smart_Cb) makeCallback(&Tabbar::onSelected), this);
	return tabbar;
}

void Tabbar::onSelected(Evas_Object *tabbar, Elm_Object_Item *item)
{
	Selector::onSelected(item);
}
