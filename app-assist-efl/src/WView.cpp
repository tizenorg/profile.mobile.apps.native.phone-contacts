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

#include "WView.h"
#include "WControl.h"
#include "WDebugInternal.h"
#include "WDefineInternal.h"
#include <app.h>
#include <Elementary.h>
#include <memory.h>
#include <string>


#define __CLASS_NAME	"WView"


class __WViewImpl
{
public:
	__WViewImpl();
	~__WViewImpl();
public:
	//
	char* __title;
	WWindow* __window;

	WNaviframe* __naviframe;
	Elm_Object_Item* __naviItem;

	std::function<void (Elm_Object_Item*)> __pushedHandler;

};
__WViewImpl::__WViewImpl()
{
	__title = NULL;
	__window = NULL;
	__naviframe = NULL;
	__naviItem = NULL;
	__pushedHandler = NULL;
}

__WViewImpl::~__WViewImpl()
{
	free(__title);
}

WView::WView()
{
	WHIT();
	__pv = new __WViewImpl();
}

WView::WView( const char* name ) : __pv( new __WViewImpl() )
{
	WHIT();
	setName( name);
}

WView::~WView()
{
	delete __pv;
}

const char* WView::getClassName()
{
	return __CLASS_NAME;
}

bool WView::create(Evas_Object* parent, void* param)
{
	if (WControl::create(parent, param)) {
		evas_object_data_set(getEvasObj(), WKEY_VIEW_INSTANCE, this );
		return true;
	}

	return false;
}

void WView::destroy()
{
	if (__pv->__naviItem)
	{
		elm_object_item_del(__pv->__naviItem);
		return;
	}
	WControl::destroy();
}

void WView::setTitle( const char* title )
{
	free( __pv->__title);
	__pv->__title = title ? strdup(title) : NULL;
}
const char* WView::getTitle()
{
	return __pv->__title;
}

void WView::setOnPushed( const std::function<void (Elm_Object_Item*)>& handlerFunc )
{
	__pv->__pushedHandler = handlerFunc;
}

bool WView::popOut()
{
	if( getEvasObj() == NULL)
	{
		WWARNING("Not created view! No Evas Object!");
		return false;
	}
	if( getNaviframe() == NULL)
	{
		WWARNING("Not pushed to naviframe!");
		return false;
	}
// "elm_object_item_widget_get" does not return right evas object!!! 2014.7.4
//	if( getEvasObj() != elm_object_item_widget_get( elm_naviframe_top_item_get(getNaviframe()->getEvasObj()) ))
//	{
//		WASSERT_EX(0,"This view is not on Top of Naviframe!");
//		return false;
//	}
	if( __pv->__naviItem != elm_naviframe_top_item_get(getNaviframe()->getEvasObj()) )
	{
		elm_object_item_del(__pv->__naviItem);
	}

	destroyPopup(); // Before popping out view, pop-up is destroyed, if it has.Because pop-up is disappeared too late.

	elm_naviframe_item_pop(getNaviframe()->getEvasObj());
	return true;
}


WWindow* WView::getWindow()
{
	return __pv->__window;
}

bool WView::onPop()
{
	return true;
}

//void WView::onBecomeTop()
//{
//}
//
//void WView::onExpose()
//{
//}

WNaviframe* WView::getNaviframe()
{
	if( __pv->__naviframe == NULL)
	{
		WWARNING("naviframe does not exist!");
	}

	return __pv->__naviframe;
}

Elm_Object_Item* WView::getNaviItem()
{
	if( __pv->__naviItem == NULL)
	{
		WWARNING("naviItem does not exist!");
	}

	return __pv->__naviItem;
}

void WView::onPushed(Elm_Object_Item* naviItem)
{

}

Elm_Object_Item* WView::onPushToNaviframe( Evas_Object* naviFrame)
{
	return NULL;
}


Evas_Object* WView::onCreate( Evas_Object* parent, void* viewParam )
{
	Evas_Object* layout = elm_layout_add(parent);
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	#if defined RESDIR
		const char* path = RESDIR;
	#else
		char* path = app_get_resource_path();
	#endif
	
	WINFO("app_get_resource_path: [%s]", path);
	if( path == NULL )
	{
		WERROR("Failed to get resource path=NULL");
		return layout;
	}
	std::string edjPath(path);
	
	#if !defined(RESDIR)
		free(path);
	#endif
	
	edjPath += "/edje/app-assist-efl.edj";

	if( elm_layout_file_set(layout, edjPath.c_str(), "default-view") == EINA_TRUE)
	{
		Evas_Object* rect = evas_object_rectangle_add(evas_object_evas_get(layout));
		evas_object_color_set(rect, rand() % 256, rand() % 256, rand() % 256, 255);
		evas_object_size_hint_weight_set(rect, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_size_hint_align_set(rect, EVAS_HINT_FILL, EVAS_HINT_FILL);

		elm_object_part_content_set(layout, "rect", rect);
		if( getName() != NULL )
			elm_object_part_text_set(layout, "name", getName());

	}
	else
	{
		WERROR("Fail to set layout. Check EDJ file(%s)", edjPath.c_str());
	}

	evas_object_show(layout);

	return layout;
}

void WView::onSetAsPageContent(Elm_Object_Item* parentNaviItem)
{

}

bool WView::enableMoreButton( Elm_Object_Item* naviItem, Evas_Smart_Cb clickedCb, void* userData )
{
	if( getNaviframe() == NULL )
	{
		WERROR("naviframe is not set");
		return false;
	}
	Evas_Object *btn = elm_button_add( getNaviframe()->getEvasObj() );
	if (!btn) return false;
	elm_object_style_set(btn, "naviframe/more/default");
	evas_object_show(btn);
	evas_object_smart_callback_add(btn, "clicked", clickedCb, userData);
	elm_object_item_part_content_set(naviItem, "toolbar_more_btn", btn);
	return true;
}

bool WView::__callPushedHandlerFunc(Elm_Object_Item* naviItem)
{
	if( __pv->__pushedHandler )
	{
		__pv->__pushedHandler(naviItem);
		return true;
	}
	return false;
}

WView* WView_getInstanceFromEvasObj(Evas_Object* obj)
{
	return (WView*)evas_object_data_get(obj, WKEY_VIEW_INSTANCE);
}

void WView_setContainerProperties( WView* view, WWindow* win, WNaviframe* naviframe )
{
	view->__pv->__window = win;
	view->__pv->__naviframe = naviframe;
}

void WView_setContainerNaviItemProperty( WView* view, Elm_Object_Item* item )
{
	view->__pv->__naviItem = item;
}
