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

#include "WToastPopup.h"
#include <string>
#include <Elementary.h>
#include <efl_extension.h>
#include "WDebugInternal.h"

#define __DEFAULT_TIME_OUT	2.0

class __WToastPopupImpl
{
public:
	__WToastPopupImpl(const char* text, double timeout );
	~__WToastPopupImpl();
public:
	//
	std::string __text;
	double __timeout;
};

__WToastPopupImpl::__WToastPopupImpl(const char* text, double timeout)
{
	__text = text;
	__timeout = timeout;
}

__WToastPopupImpl::~__WToastPopupImpl()
{

}

WToastPopup::WToastPopup(const char* text) : __pv( new __WToastPopupImpl( text, __DEFAULT_TIME_OUT ) )
{

}

WToastPopup::WToastPopup(const char* text, double timeout) : __pv( new __WToastPopupImpl( text, timeout ) )
{

}

WToastPopup::~WToastPopup()
{
	delete __pv;
}


Evas_Object* WToastPopup::onCreate(Evas_Object* parent, void* param)
{
	WHIT();

	Evas_Object* popup = NULL;
	popup = elm_popup_add( parent );

	eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK, eext_popup_back_cb, NULL);

	elm_object_style_set(popup, "toast");
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_text_set(popup,"elm.text", __pv->__text.c_str());
	elm_popup_timeout_set(popup, __pv->__timeout);
	evas_object_smart_callback_add(popup, "block,clicked",
			[](void* data, Evas_Object* obj, void* event_info) { evas_object_del(obj); }, NULL );

	evas_object_smart_callback_add(popup, "timeout",
			[](void* data, Evas_Object* obj, void* event_info) { evas_object_del(obj); }, NULL );

	evas_object_show(popup);

	return popup;
}

void WToastPopup::onDestroy()
{
}


