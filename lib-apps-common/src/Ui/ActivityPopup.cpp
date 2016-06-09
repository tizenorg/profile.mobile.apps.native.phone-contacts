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

#include "Ui/ActivityPopup.h"
#include "Ui/Window.h"

#include "App/Path.h"
#include "AppsCommonPopup.h"

#include <efl_extension.h>

using namespace Ui;

ActivityPopup::ActivityPopup(Size size)
	: m_Size(size), m_Layout(nullptr)
{
	setBackCallback([]{ return false; });
}

void ActivityPopup::setText(const char *text)
{
	elm_object_translatable_part_text_set(m_Layout, "elm.text", text);
}

Evas_Object *ActivityPopup::onCreate(Evas_Object *parent)
{
	static struct {
		const char *layout;
		const char *progress;
	} styles[] = {
		/* SizeSmall =  */ { GROUP_ACTIVITY_SMALL,  "process_small"  },
		/* SizeMedium = */ { GROUP_ACTIVITY_MEDIUM, "process_medium" }
	};

	Evas_Object *popup = Popup::onCreate(parent);
	auto &style = styles[m_Size];

	m_Layout = elm_layout_add(popup);
	elm_layout_file_set(m_Layout, App::getResourcePath(APPS_COMMON_POPUP_EDJ).c_str(), style.layout);
	elm_object_content_set(popup, m_Layout);

	Evas_Object *progressbar = elm_progressbar_add(m_Layout);
	elm_object_style_set(progressbar, style.progress);
	elm_progressbar_pulse_set(progressbar, EINA_TRUE);
	elm_progressbar_pulse(progressbar, EINA_TRUE);
	elm_object_content_set(m_Layout, progressbar);

	return popup;
}
