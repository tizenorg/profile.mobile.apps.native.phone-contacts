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

#ifndef _CT_TAB_VIEW_H_
#define _CT_TAB_VIEW_H_

#include <vector>
#include <app.h>
#include <string>
#include <memory>
#include <Ecore.h>

#include "WPageSlideView.h"

#include "IContactsTabPageView.h"

typedef enum  {
	TAB_VIEW_NONE = -1,
	TAB_VIEW_DIALER,
	TAB_VIEW_LOGS,
	TAB_VIEW_LOGS_CHECK,
	TAB_VIEW_CONTACTS,
	TAB_VIEW_MAX,
} CtTabSubView;

class WAPP_ASSIST_EXPORT CtTabView: public WPageSlideView
{
public:
	CtTabView(CtTabSubView selectedView, bool title);


public:
	void reset(CtTabSubView selectedView);
	void insert(CtTabSubView selectedView, IContactsTabPageView* subView);


private:
	virtual Evas_Object* onCreate(Evas_Object* parent, void* viewParam);
	virtual void onDestroy();
	virtual Elm_Object_Item* onPushToNaviframe( Evas_Object* naviFrame);
	virtual void onPushed(Elm_Object_Item* naviItem);
	virtual bool onPop();

private:
	bool __checkViewExist(Elm_Object_Item *tabItem);
	int __getPageIndexByViewType(int viewType);
	int __getViewTypeByPageIndex(int pageIndex);

	static void __onTabChangedCb(void* data, Evas_Object* obj, void* event_info);
	static void __pageScrollStopCb(void* data, Evas_Object* obj, void* event_info);
	static void __pageScrollStartCb(void* data, Evas_Object* obj, void* event_info);

	void __tabChanged(Elm_Object_Item *selectedItem);
	void __deleteChildView();

	static Eina_Bool __createRestViewTimerCb(void* data);

private:
	CtTabView();
	~CtTabView();

private:

	Evas_Object* __tabbar;
	Elm_Object_Item* __tabItem[TAB_VIEW_MAX];

	int __selectedView;
	int __lastSelectedView;
	bool __subViewLoaded[TAB_VIEW_MAX];
	IContactsTabPageView* __subView[TAB_VIEW_MAX];
	bool __hasTitle;
	std::vector<CtTabSubView> __tabSubView;
	Ecore_Timer *__timer;
	Ecore_Idler *__idler;

};



#endif // _PHCT_TAB_PAGE_VIEW_H_
