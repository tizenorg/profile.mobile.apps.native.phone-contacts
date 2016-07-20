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

#include "Ui/Hoversel.h"
#include "Ui/Window.h"
#include "Utils/Callback.h"

#include <efl_extension.h>

using namespace Ui;

Elm_Object_Item *Hoversel::addItem(const char *text, int value)
{
	Elm_Object_Item *item = elm_hoversel_item_add(getEvasObject(), text,
			nullptr, ELM_ICON_NONE, nullptr, (void *) (long) value);
	elm_object_item_text_translatable_set(item, EINA_TRUE);
	return item;
}

void Hoversel::setText(const char *text)
{
	elm_object_translatable_text_set(getEvasObject(), text);
}

void Hoversel::setSelectedCallback(SelectedCallback callback)
{
	m_OnSelected = std::move(callback);
}

Evas_Object *Hoversel::onCreate(Evas_Object *parent)
{
	Evas_Object *hoverParent = parent;
	Window *window = findParent<Window>(parent);
	if (window) {
		hoverParent = window->getBaseLayout();
	}

	Evas_Object *hoversel = elm_hoversel_add(parent);
	elm_hoversel_hover_parent_set(hoversel, hoverParent);

	evas_object_smart_callback_add(hoversel, "selected",
			(Evas_Smart_Cb) makeCallback(&Hoversel::onSelected), this);
	evas_object_smart_callback_add(hoversel, "expanded",
			&Hoversel::onExpanded, this);
	evas_object_smart_callback_add(hoversel, "dismissed",
			&Hoversel::onDismissed, this);

	return hoversel;
}

void Hoversel::onSelected(Evas_Object *hoversel, Elm_Object_Item *item)
{
	int value = (long) elm_object_item_data_get(item);
	if (!m_OnSelected || m_OnSelected(value)) {
		setText(elm_object_item_translatable_text_get(item));
	}
}

void Hoversel::onExpanded(void *data, Evas_Object *hoversel, void *eventInfo)
{
	eext_object_event_callback_add(hoversel, EEXT_CALLBACK_BACK,
			&Hoversel::onBackPressed, nullptr);
}

void Hoversel::onDismissed(void *data, Evas_Object *hoversel, void *eventInfo)
{
	eext_object_event_callback_del(hoversel, EEXT_CALLBACK_BACK,
			&Hoversel::onBackPressed);
}

void Hoversel::onBackPressed(void *data, Evas_Object *hoversel, void *eventInfo)
{
	elm_hoversel_hover_end(hoversel);
}
