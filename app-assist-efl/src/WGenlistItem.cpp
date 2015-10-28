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

#include "WGenlistItem.h"
#include <Elementary.h>
#include <memory.h>
#include "WDebugInternal.h"
#include "WDefineInternal.h"
#include "WView.h"


class __WGenlistItemImpl
{
public:
	__WGenlistItemImpl() {
		__objectItem = NULL; }
	~__WGenlistItemImpl(){};
public:
	Elm_Object_Item* __objectItem;

};

WGenlistItem::WGenlistItem() : __pv( new __WGenlistItemImpl() )
{
}

WGenlistItem::~WGenlistItem()
{
	delete __pv;
}

Elm_Genlist_Item_Class* WGenlistItem::getItemClassNew()
{
	return NULL;
}

Elm_Genlist_Item_Class* WGenlistItem::getItemClassStatic()
{
	return NULL;
}

void WGenlistItem::onSelect( Elm_Object_Item* objItem )
{
}


Elm_Object_Item* WGenlistItem::getElmObjectItem()
{
	return __pv->__objectItem;
}

void WGenlistItem::setElmObjectItem( Elm_Object_Item* objItem )
{
	__pv->__objectItem = objItem;
}


void WGenlistItem::selectCb(void *data, Evas_Object *obj, void *event_info)
{
	Elm_Object_Item* genlistItem = (Elm_Object_Item*) event_info;
	elm_genlist_item_selected_set(genlistItem, EINA_FALSE);

	auto item = (WGenlistItem*)data;
	item->onSelect( genlistItem );
}


bool WGenlistItem::attachToView( WControl* popup )
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
