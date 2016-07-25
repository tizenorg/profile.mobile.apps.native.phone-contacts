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

#include "Ui/ProcessPopup.h"
#include "Ui/Window.h"

#include "App/Path.h"
#include "Utils/Callback.h"
#include "AppsCommonPopup.h"

using namespace Ui;

ProcessPopup::ProcessPopup(Size size, double showDelayTime, double showMinTime)
	: Popup(false),
	  m_Size(size), m_Layout(nullptr),
	  m_ShowDelayTimer(nullptr), m_ShowMinTimer(nullptr),
	  m_IsDestroyPending(false)
{
	m_ShowDelayTimer = ecore_timer_add(showDelayTime,
			makeCallback(&ProcessPopup::onShowDelayElapsed), this);
	ecore_timer_freeze(m_ShowDelayTimer);
	m_ShowMinTimer = ecore_timer_add(showMinTime,
			makeCallback(&ProcessPopup::onShowMinElapsed), this);
	ecore_timer_freeze(m_ShowMinTimer);

	setBackCallback([this] {
		return m_IsDestroyPending;
	});
}

ProcessPopup::~ProcessPopup()
{
	ecore_timer_del(m_ShowDelayTimer);
	ecore_timer_del(m_ShowMinTimer);
}

ProcessPopup *ProcessPopup::create(Evas_Object *parent, const char *text, Size size)
{
	ProcessPopup *popup = new ProcessPopup(size);
	popup->create(parent);
	popup->setText(text);
	return popup;
}

void ProcessPopup::close()
{
	if (m_ShowDelayTimer) {
		delete this;
	} else if (m_ShowMinTimer) {
		m_IsDestroyPending = true;
	} else {
		Popup::close();
	}
}

void ProcessPopup::setText(const char *text)
{
	elm_object_translatable_part_text_set(m_Layout, "elm.text", text);
}

Evas_Object *ProcessPopup::onCreate(Evas_Object *parent)
{
	static struct {
		const char *layout;
		const char *progress;
	} styles[] = {
		/* SizeSmall  = */ { GROUP_PROCESS_SMALL,  "process_small"  },
		/* SizeMedium = */ { GROUP_PROCESS_MEDIUM, "process_medium" }
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

	ecore_timer_thaw(m_ShowDelayTimer);
	return popup;
}

Eina_Bool ProcessPopup::onShowDelayElapsed()
{
	evas_object_show(getEvasObject());
	ecore_timer_thaw(m_ShowMinTimer);

	m_ShowDelayTimer = nullptr;
	return EINA_FALSE;
}

Eina_Bool ProcessPopup::onShowMinElapsed()
{
	m_ShowMinTimer = nullptr;
	if (m_IsDestroyPending) {
		Popup::close();
	}
	return EINA_FALSE;
}
