/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
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

#include "Ui/Window.h"
#include "Ui/View.h"
#include "Utils/Callback.h"

#include <efl_extension.h>

using namespace Ui;

Window::Window()
	: m_Conform(nullptr), m_Layout(nullptr), m_MainView(nullptr)
{
}

Evas_Object *Window::onCreate(Evas_Object *parent)
{
	Evas_Object *window = elm_win_add(nullptr, nullptr, ELM_WIN_BASIC);
	elm_win_indicator_mode_set(window, ELM_WIN_INDICATOR_SHOW);
	elm_win_conformant_set(window, EINA_TRUE);
	evas_object_show(window);

	Evas_Object *bg = elm_bg_add(window);
	evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(window, bg);
	evas_object_show(bg);

	m_Conform = elm_conformant_add(window);
	evas_object_size_hint_weight_set(m_Conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(window, m_Conform);
	evas_object_show(m_Conform);

	m_Layout = elm_layout_add(m_Conform);
	elm_layout_theme_set(m_Layout, "layout", "application", "default");
	evas_object_size_hint_weight_set(m_Layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_content_set(m_Conform, m_Layout);
	evas_object_show(m_Layout);

	eext_object_event_callback_add(m_Layout, EEXT_CALLBACK_BACK,
			makeCallback(&Window::onBackPressed), this);
	eext_object_event_callback_add(m_Layout, EEXT_CALLBACK_MORE,
			makeCallback(&Window::onMenuPressed), this);

	return window;
}

Evas_Object *Window::getConformant() const
{
	return m_Conform;
}

Evas_Object *Window::getBaseLayout() const
{
	return m_Layout;
}

void Window::setBackCallback(BackCallback callback)
{
	m_OnBackPressed = std::move(callback);
}

void Window::attachView(View *view)
{
	m_MainView = view;
	elm_object_part_content_set(m_Layout, "elm.swallow.content", view->create(m_Layout));
}

void Window::onBackPressed(Evas_Object *obj, void *eventInfo)
{
	if (!m_MainView || m_MainView->onBackPressed()) {
		if (!m_OnBackPressed || m_OnBackPressed()) {
			elm_win_lower(getEvasObject());
		}
	}
}

void Window::onMenuPressed(Evas_Object *obj, void *eventInfo)
{
	if (m_MainView) {
		m_MainView->onMenuPressed();
	}
}
