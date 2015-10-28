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

#include "WButton.h"
#include <Ecore.h>
#include <Elementary.h>

#define __CLASS_NAME	"WButton"

class __WButtonImpl
{
public:
	__WButtonImpl();
	~__WButtonImpl();

private:
	friend WButton;

	void resetTimer();

	static void onMouseDown(void *data, Evas *evas, Evas_Object *obj, void *event_info);
	static void onMouseUp(void *data, Evas *evas, Evas_Object *obj, void *event_info);
	static void onMouseOut(void *data, Evas *evas, Evas_Object *obj, void *event_info);
	static Eina_Bool onTimeout(void *data);

	Ecore_Timer *__timer;
	bool __isLongpressed;

	std::function<void(WButton&)> __onPressed;
	std::function<bool(WButton&)> __onLongpressed;
};

__WButtonImpl::__WButtonImpl()
	: __timer(nullptr), __isLongpressed(false)
{
}

__WButtonImpl::~__WButtonImpl()
{
	resetTimer();
}

void __WButtonImpl::resetTimer()
{
	ecore_timer_del(__timer);
	__timer = nullptr;
}

void __WButtonImpl::onMouseDown(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
	WButton *button = (WButton*) data;
	button->__pv->__isLongpressed = false;
	button->__pv->__timer = ecore_timer_add(elm_config_longpress_timeout_get(),
		&__WButtonImpl::onTimeout, button);
}

void __WButtonImpl::onMouseUp(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
	WButton *button = (WButton*) data;
	Evas_Event_Mouse_Up *e = (Evas_Event_Mouse_Up*) event_info;

	if (!button->__pv->__isLongpressed) {
		int x = 0, y = 0, w = 0, h = 0;
		evas_object_geometry_get(obj, &x, &y, &w, &h);

		if (e->output.x >= x && e->output.x <= x + w
		 && e->output.y >= y && e->output.x <= y + h) {
			if (button->__pv->__onPressed) {
				button->__pv->__onPressed(*button);
			}
		}

		button->__pv->resetTimer();
	}
}

Eina_Bool __WButtonImpl::onTimeout(void *data)
{
	WButton *button = (WButton*) data;
	if (button->__pv->__onLongpressed) {
		button->__pv->__isLongpressed = button->__pv->__onLongpressed(*button);
	}

	button->__pv->resetTimer();
	return ECORE_CALLBACK_CANCEL;
}

void __WButtonImpl::onMouseOut(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
	WButton *button = (WButton*) data;
	button->__pv->resetTimer();
}

WButton::WButton()
{
	__pv = new __WButtonImpl();
}

WButton::~WButton()
{
	delete __pv;
}

bool WButton::create(Evas_Object *parent, void *param)
{
	bool result = WControl::create(parent, param);

	Evas_Object *button = getEvasObj();
	evas_object_event_callback_add(button, EVAS_CALLBACK_MOUSE_DOWN,
			&__WButtonImpl::onMouseDown, this);
	evas_object_event_callback_add(button, EVAS_CALLBACK_MOUSE_UP,
			&__WButtonImpl::onMouseUp, this);
	evas_object_event_callback_add(button, EVAS_CALLBACK_MOUSE_OUT,
			&__WButtonImpl::onMouseOut, this);

	return result;
}

const char *WButton::getClassName()
{
	return __CLASS_NAME;
}

void WButton::setOnPressed(std::function<void(WButton&)> callback)
{
	__pv->__onPressed = std::move(callback);
}

void WButton::setOnLongpressed(std::function<bool(WButton&)> callback)
{
	__pv->__onLongpressed = std::move(callback);
}

Evas_Object *WButton::onCreate(Evas_Object *parent, void *param)
{
	Evas_Object *button = elm_button_add(parent);
	return button;
}
