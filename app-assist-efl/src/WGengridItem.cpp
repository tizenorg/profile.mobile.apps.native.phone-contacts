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

#include "WGengridItem.h"
#include <Elementary.h>
#include <memory.h>
#include "WDebugInternal.h"
#include "WDefineInternal.h"
#include "WView.h"


class __WGengridItemImpl
{
public:
	__WGengridItemImpl() {
		__objectItem = NULL; }
	~__WGengridItemImpl(){};
public:
	Elm_Object_Item* __objectItem;

};

WGengridItem::WGengridItem() : __pv( new __WGengridItemImpl() )
{
}

WGengridItem::~WGengridItem()
{
	delete __pv;
}

Elm_Gengrid_Item_Class* WGengridItem::getItemClassNew()
{
	return NULL;
}

Elm_Gengrid_Item_Class* WGengridItem::getItemClassStatic()
{
	return NULL;
}

void WGengridItem::onSelect( Elm_Object_Item* objItem )
{
}


Elm_Object_Item* WGengridItem::getElmObjectItem()
{
	return __pv->__objectItem;
}

void WGengridItem::setElmObjectItem( Elm_Object_Item* objItem )
{
	__pv->__objectItem = objItem;
}


void WGengridItem::selectCb(void *data, Evas_Object *obj, void *event_info)
{
	auto item = (WGengridItem*)data;
	Elm_Object_Item* gengridItem = (Elm_Object_Item*) event_info;
	elm_gengrid_item_selected_set(gengridItem, EINA_FALSE);

	item->onSelect( gengridItem );
}


bool WGengridItem::attachToView( WControl* popup )
{
	if( __pv->__objectItem == NULL )
	{
		WERROR("object item is not created!");
		return false;
	}

	Evas_Object* obj = elm_object_item_widget_get( __pv->__objectItem );
	while( (obj = elm_object_parent_widget_get( obj )) != NULL )
	{
		WView* view = WView_getInstanceFromEvasObj( obj );
		if( view != NULL )
		{
			view->attachPopup( popup );
			return true;
		}
	}
	WERROR("Can not find view instance!");
	return false;
}
