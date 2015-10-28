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

////#include <efl_assist.h>

#include "ContactsDebug.h"
#include "PhLogDropDownCtxPopup.h"


PhLogDropDownCtxPopup::PhLogDropDownCtxPopup()
{

}

Evas_Object* PhLogDropDownCtxPopup::onCreate(Evas_Object* parent, void* viewParam)
{
	WHIT();

	__parentButton = parent;
	Evas_Object *popup = elm_ctxpopup_add(elm_object_top_widget_get(__parentButton));
	elm_object_style_set(popup, "dropdown/list");
	//ea_object_event_callback_add(popup, EA_CALLBACK_BACK, ea_ctxpopup_back_cb, NULL);
	evas_object_smart_callback_add(popup,"dismissed", [](void *data, Evas_Object *obj, void *event_info){
		evas_object_del(obj);
	}, NULL);

	elm_ctxpopup_direction_priority_set(popup, ELM_CTXPOPUP_DIRECTION_DOWN, ELM_CTXPOPUP_DIRECTION_UNKNOWN, ELM_CTXPOPUP_DIRECTION_UNKNOWN, ELM_CTXPOPUP_DIRECTION_UNKNOWN);

	return popup;
}

void PhLogDropDownCtxPopup::onDestroy()
{
	WHIT();
}

PhLogDropDownCtxPopup::~PhLogDropDownCtxPopup()
{

}
Elm_Object_Item* PhLogDropDownCtxPopup::appendItem( const char *label, const std::function< void()>& selectCb )
{
	__vSelectItemCb.push_back(selectCb);
	Elm_Object_Item *item = elm_ctxpopup_item_append(getEvasObj(), label, NULL, __popupSelectCb, (void*)(__vSelectItemCb.size() - 1));
	WPRET_VM( NULL == item, NULL, "elm_ctxpopup_item_append");

	return item;
}

void PhLogDropDownCtxPopup::showPopup()
{
	Evas_Coord x, y, w, h;
	evas_object_geometry_get(__parentButton, &x, &y, &w, &h);
	evas_object_move(getEvasObj(), x + (w / 2), y + h + 1);

	evas_object_show(getEvasObj());
}

void PhLogDropDownCtxPopup::__popupSelectCb( void* data, Evas_Object* obj, void* event_info )
{
	WENTER();
	PhLogDropDownCtxPopup* self = (PhLogDropDownCtxPopup*)WControl_getInstanceFromEvasObj(obj);
	if (self) {
		const unsigned int itemIndex = (long) data;
		self->__vSelectItemCb[itemIndex]();
	}

	evas_object_del(obj);
}

