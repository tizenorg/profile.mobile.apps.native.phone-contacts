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

#include "Ui/Button.h"
#include "Utils/Callback.h"

#include <Ecore.h>

using namespace Ui;

Button::Button()
	: m_Timer(nullptr), m_IsLongpressed(false)
{
}

Button::~Button()
{
	resetTimer();
}

void Button::setPressedCallback(std::function<void(Button&)> callback)
{
	m_OnPressed = std::move(callback);
}

void Button::setLongpressedCallback(std::function<bool(Button&)> callback)
{
	m_OnLongpressed = std::move(callback);
}

Evas_Object *Button::onCreate(Evas_Object *parent)
{
	Evas_Object *button = elm_button_add(parent);
	return button;
}

void Button::onCreated()
{
	evas_object_event_callback_add(getEvasObject(), EVAS_CALLBACK_MOUSE_DOWN,
			makeCallback(&Button::onMouseDown), this);
	evas_object_event_callback_add(getEvasObject(), EVAS_CALLBACK_MOUSE_UP,
			makeCallback(&Button::onMouseUp), this);
	evas_object_event_callback_add(getEvasObject(), EVAS_CALLBACK_MOUSE_OUT,
			makeCallback(&Button::onMouseOut), this);
}

void Button::onMouseDown(Evas *evas, Evas_Object *obj, void *eventInfo)
{
	m_IsLongpressed = false;
	m_Timer = ecore_timer_add(elm_config_longpress_timeout_get(),
		makeCallback(&Button::onTimeout), this);
}

void Button::onMouseUp(Evas *evas, Evas_Object *obj, void *eventInfo)
{
	Evas_Event_Mouse_Up *e = (Evas_Event_Mouse_Up *) eventInfo;

	if (!m_IsLongpressed) {
		int x = 0, y = 0, w = 0, h = 0;
		evas_object_geometry_get(obj, &x, &y, &w, &h);

		if (m_OnPressed) {
			if (e->output.x >= x && e->output.x <= x + w
			 && e->output.y >= y && e->output.x <= y + h) {
				m_OnPressed(*this);
			}
		}

		resetTimer();
	}
}

void Button::onMouseOut(Evas *evas, Evas_Object *obj, void *eventInfo)
{
	resetTimer();
}

void Button::resetTimer()
{
	ecore_timer_del(m_Timer);
	m_Timer = nullptr;
}

Eina_Bool Button::onTimeout()
{
	if (m_OnLongpressed) {
		m_IsLongpressed = m_OnLongpressed(*this);
	}

	resetTimer();
	return ECORE_CALLBACK_CANCEL;
}
