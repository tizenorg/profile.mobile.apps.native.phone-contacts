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

#include "WMenuPopup.h"
#include "WDebugInternal.h"
#include <efl_extension.h>
#include <vector>

class __WMenuPopupImpl
{
public:
	__WMenuPopupImpl();
	~__WMenuPopupImpl();
public:
	//
	Evas_Object* __win;
	Evas_Object* __navi;
	Evas_Object* __popup;
	std::vector<std::function< void()>> __vSelectItemCb;

	void __popupMove();

	static void __popupSelectCb(void* data, Evas_Object* obj, void* event_info);
	static void __popupResizeCb(void* data, Evas* e, Evas_Object* obj, void* event_info);
	static void __popupWinRotateCb(void* data, Evas_Object* obj, void* event_info);
	static void __popupDismissedCb(void* data, Evas_Object* obj, void* event_info);

};
__WMenuPopupImpl::__WMenuPopupImpl()
{
	__win = NULL;
	__navi = NULL;
	__popup = NULL;
}

__WMenuPopupImpl::~__WMenuPopupImpl()
{

}


void __WMenuPopupImpl:: __popupMove()
{
	WENTER();
	int y, w,h,w1,h1;
	int rotate;
	//
	evas_object_geometry_get(NULL, NULL, &y, &w, &h);
	elm_win_screen_size_get(__win, NULL, NULL, &w1, &h1);
	rotate = elm_win_rotation_get(__win);

	if(rotate == 90)
		evas_object_move(__popup, h1/2, w1);
	else if(rotate == 270)
		evas_object_move(__popup, h1/2, w1);
	else
		evas_object_move(__popup, w1/2, h1);

	evas_object_show(__popup);
}

void __WMenuPopupImpl::__popupSelectCb( void* data, Evas_Object* obj, void* event_info )
{
	WENTER();
	auto p = (WMenuPopup*)WControl_getInstanceFromEvasObj(obj);
	if (p) {
		const unsigned int itemIndex = (long) data;
		p->__pv->__vSelectItemCb[itemIndex]();
	}

	evas_object_del(obj);
}

void __WMenuPopupImpl::__popupResizeCb(void* data, Evas* e, Evas_Object* obj, void* event_info)
{
	WENTER();
	auto p = (WMenuPopup*)data;
	p->__pv->__popupMove();
}

void __WMenuPopupImpl::__popupWinRotateCb(void* data, Evas_Object* obj, void* event_info)
{
	WENTER();
	auto p = (WMenuPopup*)data;
	p->__pv->__popupMove();
}



void __WMenuPopupImpl::__popupDismissedCb(void* data, Evas_Object* obj, void* event_info)
{
	WENTER();
	auto p = (WMenuPopup*)data;

	evas_object_del(p->getEvasObj());
}


WMenuPopup::WMenuPopup() : __pv( new __WMenuPopupImpl() )
{
	WHIT();
}

WMenuPopup::~WMenuPopup()
{
	WHIT();
	if (__pv->__popup)
	{
		evas_object_event_callback_del(__pv->__navi, EVAS_CALLBACK_RESIZE, __WMenuPopupImpl::__popupResizeCb);
		evas_object_smart_callback_del(__pv->__win, "rotation,changed",  __WMenuPopupImpl::__popupWinRotateCb);
	}
	delete __pv;
}

void WMenuPopup::prepare(Evas_Object* win, Evas_Object* naviframe)
{
	__pv->__win = win;
	__pv->__navi = naviframe;

	Evas_Object* popup = NULL;
	popup = elm_ctxpopup_add(__pv->__win);
	elm_object_style_set(popup, "more/default");
	elm_ctxpopup_auto_hide_disabled_set(popup, EINA_TRUE);

	eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK, eext_ctxpopup_back_cb, NULL);
	eext_object_event_callback_add(popup, EEXT_CALLBACK_MORE, eext_ctxpopup_back_cb, NULL);
	evas_object_event_callback_add(__pv->__navi, EVAS_CALLBACK_RESIZE, __WMenuPopupImpl::__popupResizeCb, this);
	evas_object_smart_callback_add(__pv->__win, "rotation,changed",  __WMenuPopupImpl::__popupWinRotateCb, this);
	elm_ctxpopup_direction_priority_set(popup, ELM_CTXPOPUP_DIRECTION_DOWN, ELM_CTXPOPUP_DIRECTION_UP, ELM_CTXPOPUP_DIRECTION_UNKNOWN, ELM_CTXPOPUP_DIRECTION_UNKNOWN);
	evas_object_smart_callback_add(popup, "dismissed", __WMenuPopupImpl::__popupDismissedCb, this);
	evas_object_smart_callback_add(popup, "language,changed", __WMenuPopupImpl::__popupDismissedCb, this);

	__pv->__popup = popup;

}

Elm_Object_Item* WMenuPopup::appendItem( const char* label, std::function< void()> selectCb )
{
	WASSERT(__pv->__popup);
	__pv->__vSelectItemCb.push_back(selectCb);
	return elm_ctxpopup_item_append(__pv->__popup, label, NULL, __WMenuPopupImpl::__popupSelectCb, (void*)(__pv->__vSelectItemCb.size() - 1));
}

void WMenuPopup::createMenu()
{
	WControl::create(NULL,NULL);
}

Evas_Object* WMenuPopup::onCreate(Evas_Object* parent, void* param)
{
	if(__pv->__popup == NULL)
	{
		WERROR("Not initialized!");
		return NULL;
	}

	__pv->__popupMove();
	evas_object_show(__pv->__popup);
	return __pv->__popup;
}

void WMenuPopup::onDestroy()
{
	WENTER();
}


