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

#include "Ui/Menu.h"
#include "Ui/Window.h"
#include "Utils/Callback.h"

#include <efl_extension.h>

using namespace Ui;

Elm_Object_Item *Menu::addItem(const char *text, ItemCallback callback)
{
	Elm_Object_Item *item = elm_ctxpopup_item_append(getEvasObject(), text, nullptr,
			&Menu::onItemSelect, new ItemCallback(std::move(callback)));
	elm_object_item_text_translatable_set(item, EINA_TRUE);
	elm_object_item_del_cb_set(item, &Menu::onItemDestroy);
	return item;
}

Evas_Object *Menu::onCreate(Evas_Object *parent)
{
	Window *window = static_cast<Window *>(getControl(elm_object_top_widget_get(parent)));
	if (window) {
		parent = window->getBaseLayout();
	}

	Evas_Object *menu = elm_ctxpopup_add(parent);
	elm_object_style_set(menu, "more/default");

	evas_object_smart_callback_add(menu, "dismissed",
			makeCallback(&Menu::onDismissed), this);
	eext_object_event_callback_add(menu, EEXT_CALLBACK_BACK,
			eext_ctxpopup_back_cb, nullptr);
	eext_object_event_callback_add(menu, EEXT_CALLBACK_MORE,
			eext_ctxpopup_back_cb, nullptr);

	return menu;
}

void Menu::onItemSelect(void *data, Evas_Object *obj, void *item)
{
	ItemCallback &callback = *(ItemCallback *) data;
	if (callback) {
		callback();
	}

	elm_ctxpopup_dismiss(obj);
}

void Menu::onItemDestroy(void *data, Evas_Object *obj, void *item)
{
	delete (ItemCallback *) data;
}

void Menu::onDismissed(Evas_Object *obj, void *eventInfo)
{
	delete this;
}
