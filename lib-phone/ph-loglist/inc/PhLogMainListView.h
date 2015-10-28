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

#ifndef _PH_LOG_MAIN_LIST_VIEW_H_
#define _PH_LOG_MAIN_LIST_VIEW_H_

#include <map>
#include <system_settings.h>

#include "ContactsCommon.h"

#include "PhLogListView.h"
#include "PhLogDbMgr.h"
#include "WUiTimer.h"

/**
 * @brief Phone log main list view.
 */
class WAPP_ASSIST_EXPORT PhLogMainListView : public PhLogListView
{
public:
	/**
	 * @brief Creates log main list view.
	 */
	PhLogMainListView();

protected:
	virtual	Evas_Object* onCreate( Evas_Object* parent, void* viewParam);
	virtual void onCreated(void);
	virtual void onDestroy(void);
	virtual void onSetAsPageContent(Elm_Object_Item* parentNaviItem);

private:
	void __createViewBy();
	void __resetMissedCallCount();
	void __setMoreBtn(Elm_Object_Item *naviItem);
	void __setList(void);
	void __setGenlistItcs();
	int __getListPosition();
	void __initIcuPattern();

	Evas_Object* __getCachedContents(std::string keyString);
	Evas_Object* __createListContents(std::string contentsType);

	static void __onViewByCallsCb(void *data, Evas_Object *obj, void *event_info);
	static void __selectDetailButtonCb(void *data, Evas_Object *obj, void *event_info);
	static void __selectItemCb(void *data, Evas_Object *obj, void *event_info);
	static void __genlistUnswallowCb(void *data, Evas_Object* obj, void *event_info);
	static void __windowFocusInCb(void *data, Evas_Object *obj, void *event_info);

	//from PhLogListView
	virtual void updateList(void);

	//from PhDbListener
	virtual void onDbChanged();

	//from IContactsTabPageView
	virtual void onTabSelect(Elm_Object_Item *naviItem);
	virtual void onTabUnselect(Elm_Object_Item *naviItem);
	virtual bool onTabPop();
private:
	virtual ~PhLogMainListView();

private:
	Elm_Genlist_Item_Class *__itcGroupIndex;
	Elm_Genlist_Item_Class *__itcMainList;
	std::list<PhLogDbMgr::RecordInfo*>* __logList;

	bool __enableViewByButton;
	PhLogDbMgr::ListType __currentListType;
	std::map<Evas_Object*,std::string> __logCacheList;

	WUiTimer* __dbUpdateTimer;
	bool __isActive;
	bool __isScrollStopped;
	bool __isUpdateImmediate;
};

#endif /* _PH_LOG_MAIN_LIST_VIEW_H_ */
