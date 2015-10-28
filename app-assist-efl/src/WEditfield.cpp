/*
 * Copyright (c) 2009-2015 Samsung Electronics Co., Ltd All Rights Reserved
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

#include "WEditfield.h"
#include <Elementary.h>

namespace
{
	void onClearButton(void *data, Evas_Object *obj, void *event_info)
	{
		elm_entry_entry_set((Evas_Object*) data, "");
	}

	void onFocus(void *data, Evas_Object *obj, void *event_info)
	{
		elm_object_signal_emit((Evas_Object*) data, "elm,state,focused", "");
	}

	void onUnfocus(void *data, Evas_Object *obj, void *event_info)
	{
		elm_object_signal_emit((Evas_Object*) data, "elm,state,unfocused", "");
	}

	void onChange(void *data, Evas_Object *obj, void *event_info)
	{
		if (elm_entry_is_empty(obj))
			elm_object_signal_emit((Evas_Object*) data, "elm,action,hide,button", "");
		else
			elm_object_signal_emit((Evas_Object*) data, "elm,action,show,button", "");
	}
}

Evas_Object *WEditfield::onCreate(Evas_Object *parent, void *param)
{
	Evas_Object *layout = elm_layout_add(parent);
	elm_layout_theme_set(layout, "layout", "editfield", "singleline");

	Evas_Object *entry = elm_entry_add(parent);
	elm_entry_single_line_set(entry, EINA_TRUE);
	elm_entry_scrollable_set(entry, EINA_TRUE);
	elm_object_part_content_set(layout, "elm.swallow.content", entry);
	evas_object_show(entry);

	Evas_Object *button = elm_button_add(parent);
	elm_object_style_set(button, "editfield_clear");
	elm_object_focus_allow_set(button, EINA_FALSE);
	elm_object_part_content_set(layout, "elm.swallow.button", button);

	evas_object_smart_callback_add(button, "clicked", onClearButton, entry);
	evas_object_smart_callback_add(entry, "focused", onFocus, layout);
	evas_object_smart_callback_add(entry, "unfocused", onUnfocus, layout);
	evas_object_smart_callback_add(entry, "changed", onChange, layout);

	evas_object_data_set(layout, "weditfield", this);

	return layout;
}

Evas_Object *WEditfield::getEntry()
{
	return elm_object_part_content_get(getEvasObj(), "elm.swallow.content");
}

Evas_Object *WEditfield::getClearButton()
{
	return elm_object_part_content_get(getEvasObj(), "elm.swallow.button");
}
