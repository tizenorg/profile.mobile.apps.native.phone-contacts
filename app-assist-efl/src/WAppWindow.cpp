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

#include "WAppWindow.h"
#include "WWindowImpl.h"
#include "WDebugInternal.h"
#include "WDefineInternal.h"

#define __CLASS_NAME	"WAppWindow"

class __WAppWindowImpl : public __WWindowImpl
{
	public:
		__WAppWindowImpl(const char* inName, Elm_Win_Type inWinType);
		~__WAppWindowImpl();

	public:
		char* __name;
		Elm_Win_Type __winType;
};

__WAppWindowImpl::__WAppWindowImpl(const char* name, Elm_Win_Type winType)
	: __name(NULL),
	  __winType(winType)
{
	WASSERT( name );
	__name = strdup(name);
}

__WAppWindowImpl::~__WAppWindowImpl()
{
	free(__name);
}

WAppWindow::WAppWindow(const char* name, Elm_Win_Type winType)
	: WWindow(new __WAppWindowImpl(name, winType))
{
}

const char* WAppWindow::getClassName() const
{
	return __CLASS_NAME;
}

const char* WAppWindow::getName() const
{
	return static_cast<__WAppWindowImpl*>(__pv)->__name;
}

Elm_Win_Type WAppWindow::getWinType() const
{
	return static_cast<__WAppWindowImpl*>(__pv)->__winType;
}

Evas_Object* WAppWindow::doCreate(Evas_Object*& win, Evas_Object*& conf, Evas_Object*& layout)
{
	win = elm_win_add(NULL, getName(), getWinType());
	if (win == NULL)
	{
		WERROR("Fail to create win!");
		return NULL;
	}

	elm_win_title_set(win, getName());
	elm_win_conformant_set(win, EINA_TRUE);
	elm_win_autodel_set(win, EINA_TRUE);
	elm_win_indicator_mode_set(win, ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(win, ELM_WIN_INDICATOR_OPAQUE);

	Evas_Object* bg = elm_bg_add(win);
	evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(win, bg);
	evas_object_show(bg);

	conf = elm_conformant_add(win);
	evas_object_size_hint_weight_set(conf, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(win, conf);
	evas_object_show(conf);

	Evas_Object* cf_bg = elm_bg_add(conf);
	elm_object_style_set(cf_bg, "indicator/headerbg");
	elm_object_part_content_set(conf, "elm.swallow.indicator_bg", cf_bg);
	evas_object_show(cf_bg);

	layout = elm_layout_add(conf);
	elm_layout_theme_set(layout, "layout", "application", "default");
	evas_object_size_hint_weight_set( layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_content_set(conf, layout);
	evas_object_show(layout);

	return win;
}
