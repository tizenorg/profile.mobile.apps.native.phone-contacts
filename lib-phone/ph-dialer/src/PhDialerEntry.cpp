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

#include "PhDialerEntry.h"
#include <Elementary.h>
#include <efl_extension.h>

std::string PhDialerEntry::getNumber()
{
	std::string number;

	char *str = elm_entry_markup_to_utf8(elm_entry_entry_get(getEvasObj()));
	if (str) {
		number = str;
		free(str);
	}

	return number;
}

void PhDialerEntry::setNumber(const std::string &number)
{
	elm_entry_entry_set(getEvasObj(), number.c_str());
	elm_entry_cursor_line_end_set(getEvasObj());
}

void PhDialerEntry::insert(char c)
{
	char str[] = { c, '\0' };
	elm_entry_entry_insert(getEvasObj(), str);
}

void PhDialerEntry::popBack()
{
	int pos = elm_entry_cursor_pos_get(getEvasObj());
	if (pos > 0) {
		elm_entry_select_region_set(getEvasObj(), pos - 1, pos);
		elm_entry_entry_insert(getEvasObj(), "");
	}
}

void PhDialerEntry::clear()
{
	elm_entry_entry_set(getEvasObj(), "");
}

void PhDialerEntry::setOnChanged(std::function<void(PhDialerEntry&)> callback)
{
	m_Changed = std::move(callback);
}

Evas_Object *PhDialerEntry::onCreate(Evas_Object *parent, void *param)
{
	Evas_Object *entry = elm_entry_add(parent);
	elm_entry_single_line_set(entry, EINA_TRUE);
	elm_entry_scrollable_set(entry, EINA_TRUE);
	elm_entry_input_panel_enabled_set(entry, EINA_FALSE);

	static Elm_Entry_Filter_Accept_Set filter = { "+*#;,1234567890", NULL };
	elm_entry_markup_filter_append(entry, elm_entry_filter_accept_set, &filter);
	elm_entry_text_style_user_push(entry, "DEFAULT='font=Tizen:style=Light font_size=60 color=#fff align=center'");

	eext_entry_selection_back_event_allow_set(entry, EINA_TRUE);
	evas_object_smart_callback_add(entry, "changed",
			&PhDialerEntry::onChanged, this);
	return entry;
}

void PhDialerEntry::onChanged(void *data, Evas_Object *obj, void *event_info)
{
	PhDialerEntry *entry = (PhDialerEntry*) data;
	if (entry->m_Changed) {
		entry->m_Changed(*entry);
	}
}
