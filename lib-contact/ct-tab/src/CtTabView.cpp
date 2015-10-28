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

#include <app_preference.h>

#include "WPageSlideView.h"

#include "ContactsCommon.h"
#include "ContactsDebug.h"

#include "CtThemeMgr.h"
#include "CtRegistryMgr.h"
#include "CtString.h"
#include "CtTabView.h"
#include "CtListView.h"
#include "PhDialerView.h"
#include "PhLogMainListView.h"
#include "PhLogMainListCheckView.h"
//#include <ui_extension.h>

#define CT_TAB_CREATE_SUB_VIEW_TIMER 4.0
#define CT_TAB_CREATE_SUB_VIEW_MORE_TIMER 1.0

CtTabView::CtTabView(CtTabSubView selectedView, bool title)
{
	WENTER();
	__idler = NULL;
	__timer = NULL;
	__tabbar = NULL;

	for( int i=0; i<TAB_VIEW_MAX; i++) {
		__tabItem[i] = NULL;
		__subViewLoaded[i] = false;
		__subView[i] = NULL;
	}

	__hasTitle = title;
	__selectedView = selectedView;
	__lastSelectedView = selectedView;

	WLEAVE();
}

CtTabView::~CtTabView()
{
	WHIT();

	CtThemeMgr::getInstance()->destroy();
	CtRegistryMgr::getInstance()->destroy();
}

void CtTabView::insert(CtTabSubView viewType, IContactsTabPageView *subView)
{
	__subView[viewType] = subView;
	__tabSubView.push_back(viewType);
}

Evas_Object* CtTabView::onCreate(Evas_Object* parent, void* viewParam)
{
	WENTER();

	Evas_Object* layout = elm_layout_add(parent);
	elm_layout_theme_set(layout, "layout", "application", "default");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	setOnInitScroller( [this](Evas_Object* scroller)
	{
		elm_scroller_policy_set(scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
		//uxt_scroller_set_auto_scroll_enabled(scroller, EINA_FALSE);
	});

	Evas_Object* slideViewObj = WPageSlideView::onCreate(layout, NULL);

	elm_object_part_content_set(layout, "elm.swallow.content", slideViewObj);
	evas_object_smart_callback_add(getScroller(), "scroll,anim,stop", __pageScrollStopCb, this );
	evas_object_smart_callback_add(getScroller(), "scroll,drag,start", __pageScrollStartCb, this );

	evas_object_show(layout);
	WDEBUG("layout %p", layout);

	return layout;

}

void CtTabView::onDestroy()
{
	if(__idler)
		ecore_idler_del(__idler);
	__idler = NULL;

	if(__timer)
		ecore_timer_del(__timer);
	__timer = NULL;
}

Elm_Object_Item* CtTabView::onPushToNaviframe(Evas_Object* naviFrame)
{
	WENTER();

	//create tabbar
	Evas_Object* tabbar = elm_toolbar_add(naviFrame);
	elm_toolbar_shrink_mode_set(tabbar, ELM_TOOLBAR_SHRINK_EXPAND);
	elm_toolbar_transverse_expanded_set(tabbar, EINA_TRUE);
	WDEBUG("elm_toolbar_add");

	Elm_Object_Item* naviItem = NULL;
	if(__hasTitle) {
		elm_object_style_set(tabbar, "tabbar/tabbar_with_title");
		naviItem = elm_naviframe_item_push(naviFrame, NULL, NULL, NULL, getEvasObj(), "tabbar");
	}
	else {
		elm_object_style_set(tabbar, "tabbar");
		naviItem = elm_naviframe_item_push(naviFrame, NULL, NULL, NULL, getEvasObj(), "tabbar/notitle");
	}
	WDEBUG("elm_naviframe_item_push");

	elm_object_item_part_content_set(naviItem, "tabbar", tabbar);
	elm_toolbar_select_mode_set(tabbar, ELM_OBJECT_SELECT_MODE_ALWAYS);

	__tabbar = tabbar;
	for(auto iter = __tabSubView.begin();iter != __tabSubView.end();++iter) {
		const char *tabName = NULL;
		if(*iter == TAB_VIEW_DIALER)
			tabName = "IDS_KPD_ITAB3_KEYPAD";
		if(*iter == TAB_VIEW_LOGS || *iter == TAB_VIEW_LOGS_CHECK)
			tabName = "IDS_LOGS_ITAB3_LOGS";
		if(*iter == TAB_VIEW_CONTACTS)
			tabName = "IDS_PB_ITAB3_CONTACTS";

		WDEBUG("tabName %s %d", tabName, *iter);
		__tabItem[*iter] = elm_toolbar_item_append(tabbar, NULL, tabName, __onTabChangedCb, this);
		elm_object_item_domain_text_translatable_set(__tabItem[*iter], PACKAGE, EINA_TRUE);
	}
	WDEBUG("elm_toolbar_item_append");

	WLEAVE();

	return naviItem;

}

void CtTabView::onPushed(Elm_Object_Item* naviItem)
{
	WENTER();

	for(auto iter = __tabSubView.begin();iter != __tabSubView.end();iter++) {
		if(__subView[*iter])
			appendPage();
	}

	//set first show page
	if(__subView[__selectedView]) {
		setPageContent(__getPageIndexByViewType(__selectedView), __subView[__selectedView]);
		__subViewLoaded[__selectedView] = true;

		//__timer = ecore_timer_add(CT_TAB_CREATE_SUB_VIEW_TIMER, __createRestViewTimerCb, this);
	}

	elm_toolbar_item_selected_set(__tabItem[__selectedView], EINA_TRUE);

	WLEAVE();
}

bool CtTabView::onPop( )
{
	WHIT();

	bool isTabPop = false;
	for(int i = TAB_VIEW_DIALER; i < TAB_VIEW_MAX; i++)
	{
		if (__subView[i])
			isTabPop = __subView[i]->onTabPop();
		if (isTabPop == false)
			return false;
	}
	return true;
}

void CtTabView::__onTabChangedCb(void* data, Evas_Object* obj, void* event_info)
{
	WENTER();
	CtTabView* view = (CtTabView*)data;
	Elm_Object_Item *selectedItem = elm_toolbar_selected_item_get(obj);

	view->__tabChanged(selectedItem);

	WLEAVE();

}

void CtTabView::__tabChanged(Elm_Object_Item *selectedItem)
{
	WHIT();

	if(getPageCount() == 0) {
		WERROR("page item not added");
		return;
	}

	if(selectedItem != __tabItem[__selectedView]) {
		int i = 0;
		WDEBUG("%d", __selectedView);

		if(selectedItem) {
			__deleteChildView();

			if(__subView[__selectedView] && __subViewLoaded[__selectedView])
				__subView[__selectedView]->onTabUnselect(getNaviItem());

			for (i=TAB_VIEW_DIALER;i<TAB_VIEW_MAX;i++) {
				if (__tabItem[i]) {
					if (selectedItem == __tabItem[i]) {
						if(__subView[i])
						{
							setPageContent(__getPageIndexByViewType(i), __subView[i]);
							__subViewLoaded[i] = true;
							__subView[i]->onTabSelect(getNaviItem());
						}

						if (i == TAB_VIEW_DIALER || i == TAB_VIEW_LOGS)
						{
							preference_set_int(CT_KEY_LAST_SELECTED_TAB_INDEX_FOR_PHONE, i);
							WDEBUG("last selectedView saved as %d", i);
						}
						break;
					}
				}
			}

			__selectedView = i;
		}
		else {//first selected(launched)
			WDEBUG("first launched");
			if(__subView[__selectedView] && __subViewLoaded[__selectedView])
				__subView[__selectedView]->onTabSelect(getNaviItem());
		}
	}

	int pageIndex = __getPageIndexByViewType(__selectedView);

	WDEBUG("tabselect %d, pageIndex %d", __selectedView, pageIndex);

	showPage(pageIndex, false);

}

void CtTabView::__pageScrollStopCb(void* data, Evas_Object* obj, void* event_info)
{
	CtTabView* view = (CtTabView*)data;
	for(int i = TAB_VIEW_DIALER; i < TAB_VIEW_MAX; i++)	{
		if(view->__subView[i] && view->__subViewLoaded[i])
			view->__subView[i]->onTabScrollStop();
	}

	int hpage = 0;
	elm_scroller_current_page_get(obj, &hpage, NULL);
	WDEBUG("pagen %d", hpage);

	int viewType = view->__getViewTypeByPageIndex(hpage);

	elm_toolbar_item_selected_set(view->__tabItem[viewType], EINA_TRUE);
	return;
}

void CtTabView::__pageScrollStartCb(void* data, Evas_Object* obj, void* event_info)
{
	WHIT();
	CtTabView* view = (CtTabView*)data;
    for (auto iter = view->__tabSubView.begin(); iter != view->__tabSubView.end(); iter++)
    {
		WDEBUG("subView %d", *iter);
		if(view->__subView[*iter] && view->__subViewLoaded[*iter] == false) {
			view->setPageContent(view->__getPageIndexByViewType(*iter), view->__subView[*iter]);
			view->__subViewLoaded[*iter] = true;
		}
		view->__subView[*iter]->onTabScrollStart();
    }

	return;
}

bool CtTabView::__checkViewExist(Elm_Object_Item *tabItem)
{
	bool result = false;
	int i = TAB_VIEW_DIALER;
	for(i=TAB_VIEW_DIALER;i<TAB_VIEW_MAX;i++) {
		if(__tabItem[i] == tabItem) {
			if(__subView[i]) {
				result = true;
				break;
			}
		}
	}

	return result;
}

int CtTabView::__getPageIndexByViewType(int viewType)
{
	if(viewType >= TAB_VIEW_MAX  || __subView[viewType] == NULL) {
		WERROR("no sub view");
		return 0;
	}

	int i = 0;

	Elm_Object_Item *item = elm_toolbar_first_item_get(__tabbar);

	while(item) {
		if(__tabItem[viewType] == item && __subView[viewType])
			break;

		if(__checkViewExist(item) == true)
			i++;
		item = elm_toolbar_item_next_get(item);
	}

	return i;
}

int CtTabView::__getViewTypeByPageIndex(int pageIndex)
{
	int i = 0;

	Elm_Object_Item *item = elm_toolbar_first_item_get(__tabbar);

	for(i=0;i<pageIndex;i++) {
		item = elm_toolbar_item_next_get(item);
	}

	for(i=0;i<TAB_VIEW_MAX;i++) {
		if(__tabItem[i] == item)
			break;
	}

	return i;
}

void CtTabView::reset(CtTabSubView selectedView)
{
	WENTER();

	if(TAB_VIEW_LOGS == selectedView) {
		if(__subView[TAB_VIEW_LOGS_CHECK])
			selectedView = TAB_VIEW_LOGS_CHECK;
	}

	if(__selectedView != selectedView || TAB_VIEW_CONTACTS != selectedView)
	{
		WDEBUG("selectedView was %d and new %d", __selectedView, selectedView);
		__deleteChildView();
		//__selectedView = selectedView;
	}

	int i = 0;
	for(i=TAB_VIEW_DIALER;i<TAB_VIEW_MAX;i++) {
		if(__subView[i] && __subViewLoaded[i] && (selectedView != i)) {
			__subView[i]->onTabUnselect(getNaviItem());
		}
	}
	if(__subView[selectedView] && __subViewLoaded[selectedView]) {
		__subView[selectedView]->onTabSelect(getNaviItem());
	}

	int pageIndex = __getPageIndexByViewType(selectedView);

	WDEBUG("tabselect %d, pageIndex %d", selectedView, pageIndex);

	showPage(pageIndex, false);

	WDEBUG("toolbar set %d", selectedView);
	elm_toolbar_item_selected_set(__tabItem[selectedView], EINA_TRUE);

	WLEAVE();
}

void CtTabView::__deleteChildView()
{
	WHIT();

	Eina_List *itemList = elm_naviframe_items_get(getNaviframe()->getEvasObj());
	int count = eina_list_count(itemList);
	Eina_List *cursor = NULL;
	void *data = NULL;
	if(count > 1) {
		EINA_LIST_REVERSE_FOREACH(itemList, cursor, data) {
			Elm_Object_Item *item = (Elm_Object_Item*)data;
			elm_object_item_del(item);
			count--;
			if(count == 1)
				break;
		}
	}
	eina_list_free(itemList);

	if(__subView[__selectedView]) {
		Evas_Object* btn = elm_object_item_part_content_unset(getNaviItem(), "toolbar_more_btn");
		if (btn)
			evas_object_del(btn);
	}
}

Eina_Bool CtTabView::__createRestViewTimerCb(void* data)
{
	WHIT();
	CtTabView* view = (CtTabView*)data;
	view->__timer = NULL;
	static auto iter = view->__tabSubView.begin();

	if(view->__selectedView == TAB_VIEW_CONTACTS)
		//CONTACTS_LOGGING_LOG(LOGGING_LOG_CONTACT_APP); //DO NOT REMOVE!!!

    for (; iter != view->__tabSubView.end(); iter++)
    {
		if(view->__subView[*iter] && view->__subViewLoaded[*iter] == false) {
			WDEBUG("subView %d]%s setPageContent", *iter, view->__subView[*iter]->getName());
			view->setPageContent(view->__getPageIndexByViewType(*iter), view->__subView[*iter]);
			view->__subViewLoaded[*iter] = true;
		}
		else
			continue;
			
	    view->__timer = ecore_timer_add(CT_TAB_CREATE_SUB_VIEW_MORE_TIMER, view->__createRestViewTimerCb, data);
		return ECORE_CALLBACK_CANCEL;
    }
	return ECORE_CALLBACK_CANCEL;
}
