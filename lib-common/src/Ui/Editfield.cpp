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

#include "Ui/Editfield.h"

#include <Elementary.h>

using namespace Ui;

Editfield *Editfield::create(Evas_Object *parent, const char *guideText)
{
	Editfield *editfield = new Editfield();
	editfield->create(parent);
	editfield->setGuideText(guideText);
	return editfield;
}

Evas_Object *Editfield::onCreate(Evas_Object *parent)
{
	Evas_Object *layout = elm_layout_add(parent);
	elm_layout_theme_set(layout, "layout", "editfield", "singleline");

	Evas_Object *entry = elm_entry_add(parent);
	elm_entry_single_line_set(entry, EINA_TRUE);
	elm_entry_scrollable_set(entry, EINA_TRUE);
	elm_object_part_content_set(layout, "elm.swallow.content", entry);

	evas_object_smart_callback_add(entry, "changed", (Evas_Smart_Cb) onChanged, layout);
	evas_object_smart_callback_add(entry, "focused", (Evas_Smart_Cb) onFocused, layout);
	evas_object_smart_callback_add(entry, "unfocused", (Evas_Smart_Cb) onUnfocused, layout);

	Evas_Object *button = elm_button_add(parent);
	elm_object_style_set(button, "editfield_clear");
	elm_object_focus_allow_set(button, EINA_FALSE);
	elm_object_part_content_set(layout, "elm.swallow.button", button);
	evas_object_smart_callback_add(button, "clicked", (Evas_Smart_Cb) onClearPressed, entry);

	return layout;
}

void Editfield::setGuideText(const char *guideText)
{
	elm_object_part_text_set(getEntry(), "elm.guide", guideText);
}

Evas_Object *Editfield::getEntry()
{
	return elm_object_part_content_get(getEvasObject(), "elm.swallow.content");
}

Evas_Object *Editfield::getClearButton()
{
	return elm_object_part_content_get(getEvasObject(), "elm.swallow.button");
}

void Editfield::onChanged(Evas_Object *layout, Evas_Object *entry, void *eventInfo)
{
	if (elm_entry_is_empty(entry)) {
		elm_object_signal_emit(layout, "elm,action,hide,button", "");
	} else {
		elm_object_signal_emit(layout, "elm,action,show,button", "");
	}
}

void Editfield::onFocused(Evas_Object *layout, Evas_Object *entry, void *eventInfo)
{
	elm_object_signal_emit(layout, "elm,state,focused", "");
}

void Editfield::onUnfocused(Evas_Object *layout, Evas_Object *entry, void *eventInfo)
{
	elm_object_signal_emit(layout, "elm,state,unfocused", "");
}

void Editfield::onClearPressed(Evas_Object *entry, Evas_Object *button, void *eventInfo)
{
	elm_entry_entry_set(entry, "");
	elm_object_focus_set(entry, EINA_TRUE);
}
