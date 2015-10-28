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

#include "WPageSlideView.h"
#include <app.h>
#include "WDebugInternal.h"
#include "WDefineInternal.h"
#include "IWUiObject.h"
#include <list>
#include <string>


#define __CLASS_NAME	"WPageSlideView"

class __WPageSlideViewImpl
{
public:
	__WPageSlideViewImpl(WPageSlideView* host);
	~__WPageSlideViewImpl();
public:
	WPageSlideView* __b;
	//
	struct PageContent
	{
		IWUiObject* uiObj;
		Evas_Object* evasObj;
	};

	Evas_Object* __scroller;
	Evas_Object* __box;
	int __currentPage;
	int __pageWidth;
	int __pageHeight;
	std::list<PageContent>* __listPageContent;
	bool __showScroll;
	//
	std::function<void (Evas_Object*)> __initScrollerHandler;
	std::function<void()> __scrollAnimStopCb;

	bool __appendPage(IWUiObject* uiObj);
	bool __appendPage();
	bool __setPageContent(int index, IWUiObject* uiObj);

	static void __scrollCallback(void *data, Evas_Object *obj, void *event_info);
	static void __scrollAnimStopCallback(void *data, Evas_Object *obj, void *event_info);

	static void __layoutResizeCallback(void *data, Evas *e, Evas_Object *obj, void *event_info);
};


static Evas_Object* __createPageBase(Evas_Object* parent)
{
	Evas_Object* pageBase = elm_layout_add( parent);
	evas_object_size_hint_weight_set(pageBase, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	#if defined RESDIR
		const char * path = RESDIR;
	#else
		char * path = app_get_resource_path();
	#endif

	WINFO("app_get_resource_path: [%s]", path);
	if( path == NULL )
	{
		WERROR("Failed to get resource path=NULL!!!");
		return pageBase;
	}
	std::string edjPath(path);
	
	#if !defined(RESDIR)
		free(path);
	#endif
	
	edjPath += "/edje/app-assist-efl.edj";

	if( elm_layout_file_set(pageBase, edjPath.c_str(), "page") == EINA_FALSE )
	{
		WERROR("Fail to set layout. Check EDJ file(%s)", edjPath.c_str() );
		return pageBase;
	}

	Evas_Object* rect = evas_object_rectangle_add(evas_object_evas_get(pageBase));
	evas_object_color_set(rect, 0, 0, 0, 0);
	evas_object_size_hint_weight_set(rect, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(rect);

	elm_object_part_content_set(pageBase, "sizer", rect);

	evas_object_show(pageBase);
	return pageBase;
}

__WPageSlideViewImpl::__WPageSlideViewImpl(WPageSlideView* host):
	__scroller(NULL),
	__box(NULL),
	__currentPage(0),
	__pageWidth(0),
	__pageHeight(0),
	__listPageContent(NULL),
	__showScroll(true),
	__initScrollerHandler(NULL),
	__scrollAnimStopCb(NULL)
{
	__b = host;
	__listPageContent = new std::list<PageContent>;
}

__WPageSlideViewImpl::~__WPageSlideViewImpl()
{
	delete __listPageContent;
}

bool __WPageSlideViewImpl::__appendPage(IWUiObject* uiObj)
{
	if( __scroller == NULL)
	{
		WASSERT("First, create page slide view!");
		return false;
	}

	Evas_Object* pageBase = __createPageBase(__box);

	uiObj->create(pageBase, NULL);

	elm_object_part_content_set(pageBase, "sw", uiObj->getEvasObj());

	elm_box_pack_end(__box, pageBase);

	__listPageContent->push_back( PageContent{uiObj, pageBase});

	return true;
}

bool __WPageSlideViewImpl::__appendPage()
{
	if( __scroller == NULL)
	{
		WASSERT("First, create page slide view!");
		return false;
	}

	Evas_Object* pageBase = __createPageBase(__box);

	elm_box_pack_end(__box, pageBase);

	__listPageContent->push_back( PageContent{NULL, pageBase});

	return true;
}

bool __WPageSlideViewImpl::__setPageContent(int index, IWUiObject* uiObj)
{
	int count = 0;
	Evas_Object* pageBase = NULL;
	for( auto it = __listPageContent->begin(); it != __listPageContent->end(); it++,count++ )
	{
		if( index == count )
		{
			if( (*it).uiObj )
			{
				WERROR("Index is already occupied(index=%d)", index);
				return false;

			}
			(*it).uiObj = uiObj;
			pageBase = (*it).evasObj;
			break;
		}
	}
	if( count != index )
	{
		WERROR("Index is not in range(index=%d,count=%d)", index, count);
		return false;
	}

	Evas_Object *content = elm_object_part_content_get( pageBase, "sw");
	if(content == NULL) {
		uiObj->create( pageBase, NULL);
		elm_object_part_content_set( pageBase, "sw", uiObj->getEvasObj());
		return true;
	}

	return false;
}


void __WPageSlideViewImpl::__scrollCallback(void *data, Evas_Object *obj, void *event_info)
{
	__WPageSlideViewImpl* pageViewImpl = ((WPageSlideView *)data)->__pv;
	elm_scroller_current_page_get(obj, &pageViewImpl->__currentPage, NULL);
}

void __WPageSlideViewImpl::__scrollAnimStopCallback(void *data, Evas_Object *obj, void *event_info)
{
	__WPageSlideViewImpl* pageViewImpl = ((WPageSlideView *)data)->__pv;
	if (pageViewImpl->__scrollAnimStopCb)
		pageViewImpl->__scrollAnimStopCb();
}

void __WPageSlideViewImpl::__layoutResizeCallback(void *data, Evas *e, Evas_Object *obj, void *event_info)
{

	WHIT();

	WPageSlideView* pageView = (WPageSlideView *)data;
	__WPageSlideViewImpl* pv = pageView->__pv;
	int w, h;

	evas_object_geometry_get(obj, NULL, NULL, &w, &h);

	pv->__pageWidth = w;
	pv->__pageHeight = h;

	WDEBUG("%d %d", w, h);

	for( auto it = pv->__listPageContent->begin(); it != pv->__listPageContent->end(); it++)
	{
		Evas_Object* pageBase = (*it).evasObj;
		evas_object_size_hint_min_set(elm_object_part_content_get(pageBase, "sizer"), w, h);
	}

	elm_scroller_page_size_set(pv->__scroller, w, h);
	elm_scroller_page_show(pv->__scroller, pv->__currentPage, 0);
}


WPageSlideView::WPageSlideView()
{
	__pv = new __WPageSlideViewImpl(this);
}

WPageSlideView::~WPageSlideView()
{
	delete __pv;
}

void WPageSlideView::setOnScrollAnimStop(const std::function<void()>& scrollAnimStopCb)
{
	__pv->__scrollAnimStopCb = scrollAnimStopCb;
}

const char* WPageSlideView::getClassName()
{
	return __CLASS_NAME;
}

bool WPageSlideView::appendPage( WControl* content)
{
	if( __pv->__appendPage(content) == false )
	{
		WERROR("Fail to attach the page!" );
		return false;
	}

	return true;
}

bool WPageSlideView::appendPage( WView* content)
{
	if( __pv->__appendPage(content) == false )
	{
		WERROR("Fail to attach the page!" );
		return false;
	}

	WView_setContainerProperties( content, getWindow(), getNaviframe() );
	WView_setContainerNaviItemProperty( content, getNaviItem() );

	content->onSetAsPageContent( getNaviItem() );

	return true;
}

bool WPageSlideView::appendPage()
{
	if(__pv->__appendPage() == false)
	{
		WERROR("Fail to attach the page!" );
		return false;
	}

	return true;
}

bool WPageSlideView::setPageContent(int index, WView* content)
{
	if(__pv->__setPageContent(index, content) == false)
	{
		WERROR("Fail to set page content %d!", index );
		return false;
	}

	WView_setContainerProperties( content, getWindow(), getNaviframe() );
	WView_setContainerNaviItemProperty( content, getNaviItem() );

	content->onSetAsPageContent( getNaviItem() );

	return true;
}

bool WPageSlideView::setPageContent(int index, WControl* content)
{
	if(__pv->__setPageContent(index, content) == false)
	{
		WERROR("Fail to set page content %d!", index );
		return false;
	}
	return true;
}


int WPageSlideView::getPageCount()
{
	return __pv->__listPageContent->size();
}

IWUiObject* WPageSlideView::getPageContent( int index)
{
	int count = 0;
	for( auto it = __pv->__listPageContent->begin(); it != __pv->__listPageContent->end(); it++)
	{
		if( count == index)
			return (*it).uiObj;
		count++;
	}
	return NULL;
}

int WPageSlideView::getCurrentPageIndex()
{
	return __pv->__currentPage;
}

Evas_Object* WPageSlideView::getScroller()
{
	return __pv->__scroller;
}

void WPageSlideView::setShowScroll(bool show)
{
	__pv->__showScroll = show;
}

void WPageSlideView::setCurrentPage(int index)
{
	__pv->__currentPage = index;
//	elm_scroller_page_show(__pv->__scroller, __pv->__currentPage, 0);
}

void WPageSlideView::showPage(int index, bool withAnimation)
{
	__pv->__currentPage = index;
	if (withAnimation)
		elm_scroller_page_bring_in(__pv->__scroller, __pv->__currentPage, 0);
	else
		elm_scroller_page_show(__pv->__scroller, __pv->__currentPage, 0);
}

void WPageSlideView::unsetPageObject(int index)
{
	auto it = __pv->__listPageContent->begin();
	for (int i = 0; i < index; i++)
		it++;
	elm_object_part_content_unset((*it).evasObj, "sw");
}

void WPageSlideView::setPageObject(int index, WControl* control )
{
	auto it = __pv->__listPageContent->begin();
	for (int i = 0; i < index; i++)
		it++;
	elm_object_part_content_set( (*it).evasObj, "sw", control->getEvasObj());
	(*it).uiObj = control;

}


void WPageSlideView::setOnInitScroller( const std::function<void (Evas_Object*)>& handlerFunc )
{
	__pv->__initScrollerHandler = handlerFunc;
}

Evas_Object* WPageSlideView::onCreate( Evas_Object* parent, void* viewParam )
{
	WHIT();

	Evas_Object* layout = onCreateLayout( parent );
	Evas_Object* scroller = NULL;
	if( layout ) {
		scroller = elm_scroller_add( layout);
		evas_object_event_callback_add(layout, EVAS_CALLBACK_RESIZE, __WPageSlideViewImpl::__layoutResizeCallback, this);
	} else {
		scroller = elm_scroller_add( parent);
		evas_object_event_callback_add(scroller, EVAS_CALLBACK_RESIZE, __WPageSlideViewImpl::__layoutResizeCallback, this);
	}

	evas_object_smart_callback_add(scroller, "scroll.page.changed", __WPageSlideViewImpl::__scrollCallback, this);
	evas_object_smart_callback_add(scroller, "scroll,anim,stop", __WPageSlideViewImpl::__scrollAnimStopCallback, this);

	elm_scroller_loop_set( scroller, EINA_FALSE, EINA_FALSE);
	evas_object_size_hint_weight_set( scroller, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set( scroller, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_scroller_page_relative_set( scroller, 1.0, 0.0);
	elm_scroller_policy_set( scroller,
	                        __pv->__showScroll ? ELM_SCROLLER_POLICY_AUTO : ELM_SCROLLER_POLICY_OFF,
	                        ELM_SCROLLER_POLICY_OFF);
	elm_scroller_page_scroll_limit_set( scroller, 1, 0);
	elm_object_scroll_lock_y_set( scroller, EINA_TRUE);
	elm_scroller_single_direction_set(scroller, ELM_SCROLLER_SINGLE_DIRECTION_HARD);
	//
	if( __pv->__initScrollerHandler )
		__pv->__initScrollerHandler(scroller);
	else
		onInitScroller( scroller );
	//
	if (layout)	{
		elm_object_part_content_set(layout, "scroller", scroller);
	}

	Evas_Object* box = elm_box_add(scroller);

	__pv->__scroller = scroller;
	__pv->__box = box;

	elm_box_horizontal_set( box, EINA_TRUE);
	elm_object_content_set( scroller, box);
	evas_object_show( scroller);
	evas_object_show( box);

	if( layout )
		return layout;
	else
		return scroller;

}

void WPageSlideView::onInitScroller( Evas_Object* scroller )
{
}

Evas_Object* WPageSlideView::onCreateLayout( Evas_Object* parent)
{
	WHIT();
	return NULL;
}


