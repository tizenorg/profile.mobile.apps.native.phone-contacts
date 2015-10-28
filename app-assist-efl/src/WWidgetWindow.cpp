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

#include "WWidgetWindow.h"
#include "WWindowImpl.h"

#include <widget_app_efl.h>

#define __CLASS_NAME	"WWidgetWindow"

class __WWidgetWindowImpl : public __WWindowImpl
{
public:
	__WWidgetWindowImpl(widget_context_h context, int w, int h);

public:
	widget_context_h __context;
	int __width;
	int __height;
};

__WWidgetWindowImpl::__WWidgetWindowImpl(widget_context_h context, int w, int h)
	: __context(context), __width(w), __height(h)
{
}

WWidgetWindow::WWidgetWindow(widget_context_h context, int w, int h)
	: WWindow(new __WWidgetWindowImpl(context, w, h))
{
}

const char *WWidgetWindow::getClassName() const
{
	return __CLASS_NAME;
}

Evas_Object *WWidgetWindow::doCreate(Evas_Object *&win, Evas_Object *&conf, Evas_Object *&layout)
{
	__WWidgetWindowImpl *pv = static_cast<__WWidgetWindowImpl*>(__pv);

	widget_app_get_elm_win(pv->__context, &win);
	evas_object_resize(win, pv->__width, pv->__height);
	evas_object_show(win);

	conf = elm_conformant_add(win);
	evas_object_size_hint_weight_set(conf, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(win, conf);
	evas_object_show(conf);

	layout = elm_layout_add(conf);
	evas_object_resize(layout, pv->__width, pv->__height);
	elm_layout_theme_set( layout, "layout", "application", "default");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(layout);

	Evas_Object *bg = elm_bg_add(layout);
	evas_object_color_set(bg, 255, 255, 255, 0);
	evas_object_show(bg);
	elm_object_part_content_set(layout, "elm.swallow.bg", bg);

	return layout;
}
